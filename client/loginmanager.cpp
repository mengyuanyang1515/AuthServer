#include "loginmanager.h"
#include <QDebug>
#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QtEndian>
#include "../common/auth.pb.h"
#include "../common/common_def.h"
#include <QQmlContext>

LoginManager::LoginManager(QObject *parent) : QObject(parent), net_codec_(parent)
{
    tcp_client_ = new QTcpSocket(this);
    
    connect(tcp_client_, &QTcpSocket::connected, this, &LoginManager::connected);
    connect(tcp_client_, &QTcpSocket::disconnected, this, &LoginManager::disconnected);
}

void LoginManager::Initialize()
{
    net_codec_.SetMessageCallback(std::bind(&LoginManager::HandleMessage, this
                                            , std::placeholders::_1, std::placeholders::_2
                                            , std::placeholders::_3, std::placeholders::_4));
}

void LoginManager::setQmlEngine(QQmlApplicationEngine *engine)
{
    qml_engine_ = engine;
}

void LoginManager::handleRegister(const QString &username, const QString &password)
{
    qDebug() << "handlerRegister, name:" << username << " password:" << password;
    
    user_name_ = username;
    password_ = password;
    state_ = LoginState_Register;
    if (tcp_client_ == nullptr)
    {
        qDebug() << "LoginState_Register tcp_client_ is nullptr";
    }
    else
    {
        qDebug() << "LoginState_Register tcp_client_ is not nullptr";
    }
    tcp_client_->connectToHost("localhost", 5060);
}

void LoginManager::handleLogin(const QString &username, const QString &password)
{
    // 在这里处理登录逻辑
    qDebug() << "handleLogin" << " Username:" << username << " Password:" << password;
    
    user_name_ = username;
    password_ = password;
    
    state_ = LoginState_Login;
    if (tcp_client_ == nullptr)
    {
        qDebug() << "LoginState_Login tcp_client_ is nullptr tcp_client:" << tcp_client_;
    }
    else
    {
        qDebug() << "LoginState_Login tcp_client_ is not nullptr:" << tcp_client_;
    }
    qDebug() << "LoginState_Login tcp_client_ 2222222:";
    tcp_client_->connectToHost("localhost", 5060);
    
}

void LoginManager::handleQuit()
{
    
}

void LoginManager::connected()
{
    qDebug() << "连接AuthServer成功";
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    
    if (socket == nullptr)
    {
        qDebug() << "LoginManager::connnected socket is nullptr, socket:" << socket;
    }
    else
    {
        qDebug() << "LoginManager::connnected socket is not nullptr, socket:" << socket;
    }
    
    if (socket != nullptr)
    {
        net_codec_.RegisterSocket(socket);
        connect(socket, &QTcpSocket::readyRead, &net_codec_, &NetCodec::ReadyRead);     
    }

    if (state_ == LoginState_Register)
    {
        SendRegisterReq(tcp_client_);
    }
    else if (state_ == LoginState_Login)
    {
        SendLoginReq(tcp_client_);
    }
    else if (state_ == LoginState_Landing)
    {
        SendLandingReq(tcp_client_);
    }
}

void LoginManager::disconnected()
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    
    qDebug() << "连接断开, socket:" << socket;
    
    net_codec_.UnRegisterSocket(socket);
    
    //socket->deleteLater();
}



void LoginManager::HandleMessage(QTcpSocket* socket, unsigned int message_id, const char *data, int len)
{
    qDebug() << "进入到LoginManager::HandleMessage, messsage_id:" << message_id << " len:" << len;
    switch(message_id)
    {
    case MessageId::kRegisterAck:
    {
        auth::RegisterAck registerAck;
        registerAck.ParseFromString(data);
        HandleRegisterAck(registerAck);
        
        socket->close(); // 短连接，早点断开
        break;
    }
    case MessageId::kLoginAck:
    {
        auth::LoginAck loginAck;
        loginAck.ParseFromString(data);
        HandleLoginAck(socket, loginAck);
        break;
    }
    case MessageId::kLandingAck:
    {
        auth::LandingAck ladingAck;
        ladingAck.ParseFromString(data);
        HandleLandingAck(ladingAck);  // 这里是长连接，不断开，后面如果退出，后面可以退出
        break;
    }
    case MessageId::KQuitAck:
    {
        auth::QuitAck quitAck;
        quitAck.ParseFromString(data);
        HandleQuitAck(quitAck);
        break;
    }
    default:
        break;
    }
}

void LoginManager::SendRegisterReq(QTcpSocket* socket)// todo:yangmengyuan  #1
{
    qDebug() << "trace log 1 LoginManager::SendRegisterReq";
    auth::RegisterReq regReq;
    regReq.set_name(user_name_.toStdString().c_str());
    regReq.set_passwd(password_.toStdString().c_str());
    std::string data;
    regReq.SerializeToString(&data);


    net_codec_.WriteMessage(MessageId::kRegisterReq, data.c_str(), data.size(), socket);
}

void LoginManager::HandleRegisterAck(auth::RegisterAck ack)// todo:yangmengyuan  #4
{
    bool is_ok = ack.is_ok();
    std::string error_info = ack.error_info();
    
    qDebug() << "trace log 3 LoginManager::HandleRegisterAck, is_ok:" << is_ok << " error_info:" << error_info.c_str();
    
    emit registerSuccess(is_ok);
}


void LoginManager::SendLoginReq(QTcpSocket* socket)// todo:yangmengyuan #5
{
    qDebug() << "trace log 5 LoginManager::SendLoginReq";
    auth::LoginReq loginReq;
    loginReq.set_name(user_name_.toStdString().c_str());
    loginReq.set_passwd(password_.toStdString().c_str());
    std::string data;
    loginReq.SerializeToString(&data);


    net_codec_.WriteMessage(MessageId::kLoginReq, data.c_str(), data.size(), socket);
}

void LoginManager::HandleLoginAck(QTcpSocket* socket, auth::LoginAck ack)// todo:yangmengyuan #8
{
    bool is_ok = ack.is_ok();
    std::string token = ack.token();
    
    qDebug() << "trace log 8 LoginManager::HandleLoginAck, is_ok:" << is_ok << " token:" << token.c_str();
    
    socket->close();
    
    if (is_ok) {
        // 发消息到LandingServer
        state_ = LoginState_Landing;
        tcp_client_->connectToHost("localhost", 6060);
        
        token_ = token;
    } else {
        emit loginSuccess(is_ok);
    }
}

void LoginManager::SendLandingReq(QTcpSocket* socket)// todo:yangmengyuan #9
{
    qDebug() << "trace log 9 LoginManager::SendLandingReq, token:" << token_.c_str();
    auth::LandingReq landingReq;
    landingReq.set_token(token_);
    std::string data;
    landingReq.SerializeToString(&data);
    net_codec_.WriteMessage(MessageId::kLandingReq, data.c_str(), data.size(), socket);
}

void LoginManager::HandleLandingAck(auth::LandingAck ack)// todo:yangmengyuan #16
{
    bool is_ok = ack.is_ok();
    
    qDebug() << "trace log 16 Received LandingAck, is_ok:" << is_ok;
    
    emit loginSuccess(is_ok);
}


void LoginManager::SendQuitReq(QTcpSocket* socket)
{
    
}

void LoginManager::HandleQuitAck(auth::QuitAck ack)
{
    
}









