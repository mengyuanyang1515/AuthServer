#include "landing_manager.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QtEndian>
#include <functional>
#include "../common/net_codec.h"
#include "../common/auth.pb.h"

LandingManager::LandingManager(QObject *parent) : QObject(parent), net_codec_(parent){
    qDebug() << "开始landingServer的构造函数";
    tcp_server_ = new QTcpServer(this);

    connect(tcp_server_, &QTcpServer::newConnection, this, &LandingManager::newConnection);

    if (!tcp_server_->listen(QHostAddress::SpecialAddress::LocalHost, 6060)){
        qDebug() << "landing Server 启动失败";
    }
    else{
        qDebug() << "landing Server 启动成功";
    }
    
    
    tcp_client_ = new QTcpSocket(this);
    
    connect(tcp_client_, &QTcpSocket::connected, this, &LandingManager::clientConnected);
    connect(tcp_client_, &QTcpSocket::disconnected, this, &LandingManager::clientDisconnected);
}

void LandingManager::Initialize(){
    net_codec_.SetMessageCallback(std::bind(&LandingManager::HandleMessage, this
                                            , std::placeholders::_1, std::placeholders::_2
                                            , std::placeholders::_3, std::placeholders::_4));
}

void LandingManager::newConnection(){
    QTcpSocket *socket = tcp_server_->nextPendingConnection();
    
    connect(socket,&QTcpSocket::disconnected,this,&LandingManager::disconnected);
    
    connect(socket, &QTcpSocket::readyRead, &net_codec_, &NetCodec::ReadyRead);
    
    net_codec_.RegisterSocket(socket);
    
    qDebug() << "收到新的客户连接 IP:" << socket->peerAddress().toString() << "Port:" << socket->peerPort();
}

void LandingManager::disconnected(){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "landingServer 连接断开";

    net_codec_.UnRegisterSocket(socket);
    
    socket->deleteLater();
}

void LandingManager::clientConnected(){
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "从LandingServer连接到AuthServer成功，socket:" << socket << "tcp_client_:" << tcp_client_;
    if(socket != nullptr){
        net_codec_.RegisterSocket(socket);
        connect(socket,&QTcpSocket::readyRead,&net_codec_,&NetCodec::ReadyRead);
    }
    
    this->SendAuthReq(tcp_client_);
}

void LandingManager::clientDisconnected(){
    QTcpSocket *socket = (QTcpSocket*)sender();
    qDebug() << "连接断开";
    
    net_codec_.UnRegisterSocket(socket);
    
    if (socket != tcp_client_)
    {
        socket->deleteLater();    
    }
}

// 粘包结束以后的函数处理
void LandingManager::HandleMessage(QTcpSocket *socket, unsigned int message_id, const char *data, int len){
    qDebug() << "进入到LandingManager::HandleMessage, messsage_id:" << message_id << " len:" << len;
    switch(message_id)
{
    case MessageId::kLandingReq:
    {
        auth::LandingReq landingReq;
        landingReq.ParseFromString(data);
        HandleLandingReq(socket, landingReq);
        break;
    }
    case MessageId::kAuthAck:
    {
        auth::AuthAck authAck;
        authAck.ParseFromString(data);
        HandleAuthAck(authAck);
        socket->close();
        break;
    }
    case MessageId::KQuitReq:
    {
        auth::QuitReq quitReq;
        quitReq.ParseFromString(data);
        HandleQuitReq(quitReq);
        break;
    }
    default:
        break;
    }
}

void LandingManager::HandleLandingReq(QTcpSocket* socket, auth::LandingReq &req) // todo:yangmengyuan #10
{
    token_ = req.token();
    
    if (connected_clients_.find(token_) == connected_clients_.end())
    {
        connected_clients_[token_] = socket;
    }
    
    qDebug() << "trace log 10 LandingManager::HandleLandingReq" << " token:" << token_.c_str();
    
    tcp_client_->connectToHost("localhost", 5060);
}

void LandingManager::SendLandingAck(bool is_ok, const std::string& token) // todo:yangmengyuan #15
{
    qDebug() << "trace log 15 LandingManager::SendLandingAck, token:" << token.c_str();
    auth::LandingAck landingAck;
    landingAck.set_is_ok(is_ok);
    std::string data;
    landingAck.SerializeToString(&data);
    
    auto it = connected_clients_.find(token);
    if (it != connected_clients_.end())
    {
        net_codec_.WriteMessage(MessageId::kLandingAck, data.c_str(), data.size(), it->second);
    }
}


void LandingManager::HandleAuthAck(auth::AuthAck& ack)  // todo:yangmengyuan #14
{
    bool is_ok = ack.is_ok();
    std::string token = ack.token();
    
    qDebug() << "trace log 14 LandingManager::handAuthAck, is_ok:" << is_ok << " token:" << token.c_str();
    
    this->SendLandingAck(is_ok, token);
}


void LandingManager::SendAuthReq(QTcpSocket *socket) // todo:yangmengyuan #11
{    
    qDebug() << "trace log 11 LandingManager::SendAuthReq, token:" << token_.c_str() << " tcp_client:" << tcp_client_ << " socket:" << socket;
    auth::AuthReq authReq;
    authReq.set_token(token_);
    std::string data;
    authReq.SerializeToString(&data);

    net_codec_.WriteMessage(MessageId::kAuthReq, data.c_str(), data.size(), socket);
}


void LandingManager::HandleQuitReq(auth::QuitReq& req)
{
    
}

void LandingManager::SendQuitAck(auth::QuitAck& ack)
{
    
}




