#include "auth_manager.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QtEndian>
#include <map>
#include <string>
#include <functional>

#include "../common/auth.pb.h"
#include "../common/common_def.h"

std::map<std::string, std::string> g_name_pwd_map;
std::map<std::string, std::string> g_user_token_map;

AuthManager::AuthManager(QObject *parent) : QObject(parent), net_codec_(parent){
    tcp_server_ = new QTcpServer(this);

    connect(tcp_server_,&QTcpServer::newConnection,this,&AuthManager::newConnection);
    
    if(!tcp_server_->listen(QHostAddress::SpecialAddress::LocalHost,5060))
    {
        qDebug() << "Auth Server 启动失败";
    }
    else
    {
        qDebug() << "Auth Server 启动成功";
    }
}

void AuthManager::Initialize(){
    net_codec_.SetMessageCallback(std::bind(&AuthManager::HandleMessage, this
                                            , std::placeholders::_1, std::placeholders::_2
                                            , std::placeholders::_3, std::placeholders::_4));
}

void AuthManager::newConnection(){
    QTcpSocket *socket = tcp_server_->nextPendingConnection();
    
    connect(socket,&QTcpSocket::disconnected,this,&AuthManager::disconnected);
    
    connect(socket, &QTcpSocket::readyRead, &net_codec_, &NetCodec::ReadyRead);
    
    net_codec_.RegisterSocket(socket);
    
    qDebug() << "收到新的客户连接 IP:" << socket->peerAddress().toString() << "Port:" << socket->peerPort();
}

void AuthManager::disconnected(){
    QTcpSocket *socket = (QTcpSocket*)sender();
    
    qDebug() << "连接断开";
    
    net_codec_.UnRegisterSocket(socket);
    
    socket->deleteLater();
}

// 粘包结束以后的函数处理
void AuthManager::HandleMessage(QTcpSocket* socket, unsigned int message_id, const char *data, int len){
    qDebug() << "进入到HandleMessage, messsage_id:" << message_id << " len:" << len;
    switch(message_id)
    {
    case MessageId::kRegisterReq:
    {
        auth::RegisterReq registerReq;
        registerReq.ParseFromString(data);
        HandleRegisterReq(socket, registerReq);
        break;
    }
    case MessageId::kLoginReq:
    {
        auth::LoginReq loginReq;
        loginReq.ParseFromString(data);
        HandleLoginReq(socket, loginReq);
        break;
    }
    case MessageId::kAuthReq:
    {
        auth::AuthReq authReq;
        authReq.ParseFromString(data);
        HandleAuthReq(socket, authReq);
        break;
    }
    default:
        break;
    }
}

void AuthManager::HandleRegisterReq(QTcpSocket* socket, auth::RegisterReq &req) // todo:yangmengyuan  #2
{
    qDebug() << "trace log 2 AuthManager::HandleRegisterReq";
    
    std::string name = req.name();
    std::string passpwd = req.passwd();
    
    qDebug() << "服务器收到消息RegisterReq, name:" << name.c_str() << " passpwd:" << passpwd.c_str();
    auto it = g_name_pwd_map.find(name);
    bool is_ok = true;
    std::string error_info;
    if (it == g_name_pwd_map.end())
    {
        g_name_pwd_map[name] = passpwd;
        qDebug() << "注册成功";
     }
    else
    {
        // 重名
        is_ok = false;
        error_info = "重名";
        qDebug() << "注册失败，重名";
    }
    
    SendRegisterAck(socket, is_ok, error_info);
}

void AuthManager::SendRegisterAck(QTcpSocket* socket, bool is_ok, const std::string& error_info) // todo:yangmengyuan  #3
{
    qDebug() << "trace log 3 AuthManager::SendRegisterAck";
    auth::RegisterAck regAck;
    regAck.set_is_ok(is_ok);
    regAck.set_error_info(error_info);
    std::string data;
    regAck.SerializeToString(&data);

    net_codec_.WriteMessage(MessageId::kRegisterAck, data.c_str(), data.size(), socket);
}

void AuthManager::HandleLoginReq(QTcpSocket* socket, auth::LoginReq &req) // todo:yangmengyuan   #6
{
    std::string name = req.name();
    std::string passpwd = req.passwd();
    
    qDebug() << "trace log 6 AuthManager::HandleLoginReq, name:" << name.c_str() << " passpwd:" << passpwd.c_str();

    auto it = g_name_pwd_map.find(name);
    bool is_ok = false;
    if (it != g_name_pwd_map.end()  && it->second == passpwd)
    {
        is_ok = true;
    }
    
    this->SendLoginAck(socket, is_ok, name);
}

void AuthManager::SendLoginAck(QTcpSocket* socket, bool is_ok, const std::string& name) // todo:yangmengyuan  #7
{
    std::string token;
    auth::LoginAck loginAck;
    if(is_ok)
    {
        token = "111cccaaa"; // todo:yangmengyuan 增加一个生成token的算法
        g_user_token_map[token] = name;
    }
    
    qDebug() << "trace log 7 AuthManager::SendLoginAck, is_ok:" << is_ok << " token:" << token.c_str();
    
    loginAck.set_is_ok(is_ok);
    loginAck.set_token(token);
    std::string data;
    loginAck.SerializeToString(&data);

    net_codec_.WriteMessage(MessageId::kLoginAck, data.c_str(), data.size(), socket);
}

void AuthManager::HandleAuthReq(QTcpSocket* socket, auth::AuthReq &req) // todo:yangmengyuan  12
{
    std::string token = req.token();
    
    qDebug() << "trace log 12 AuthManager::HandleAuthReq,token:" << token.c_str();
    
    bool is_ok = false;
    std::string ret_name;
    auto it = g_user_token_map.find(token);
    if (it != g_user_token_map.end())
    {
        is_ok = true;
        ret_name = it->second;
        qDebug() << "AuthServer找到了token验证";
        g_user_token_map.erase(it); // 理论上token需要保存一段时间然后过期，类似于手游的登录流程，现在简单点，直接删除
    }
    
    this->SendAuthAck(socket, is_ok, ret_name);
}

void AuthManager::SendAuthAck(QTcpSocket* socket, bool is_ok, const std::string& ret_name) // todo:yangmengyuan  #13
{
    qDebug() << "trace log 13 AuthManager::SendAuthAck, is_ok:" << is_ok << " ret name:" << ret_name.c_str();
    
    auth::AuthAck auth_ack;
    auth_ack.set_is_ok(is_ok);
    auth_ack.set_name(ret_name);
    std::string data;
    auth_ack.SerializeToString(&data);

    net_codec_.WriteMessage(MessageId::kAuthAck, data.c_str(), data.size(), socket);
}









