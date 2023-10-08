#ifndef LOGINMANAGER_H
#define LOGINMANAGER_H

#include <QObject>
#include <string>
#include "../common/net_codec.h"
#include "../common/auth.pb.h"

class QQmlApplicationEngine; // 前置声明，减少编译器依赖
class QTcpSocket;
class LoginManager : public QObject
{
    Q_OBJECT
    
    enum EnterState
    {
        LoginState_None,
        LoginState_Register,
        LoginState_Login,
        LoginState_Landing,
        LoginState_Max
    };
    
signals:
    void loginSuccess(bool success);
    void registerSuccess(bool success);
    
public:
    explicit LoginManager(QObject  *parent = nullptr);
    void Initialize();
    void setQmlEngine(QQmlApplicationEngine* engine);
    
public slots:
    void handleRegister(const QString &username, const QString &password);
    void handleLogin(const QString &username, const QString &password);
    void handleQuit();
    
private slots:
    void connected();
    void disconnected();
    
private:
    void HandleMessage(QTcpSocket* socket, unsigned int message_id, const char* data, int len);
      
    void SendRegisterReq(QTcpSocket* socket);
    void HandleRegisterAck(auth::RegisterAck ack);
    
    void SendLoginReq(QTcpSocket* socket);
    void HandleLoginAck(QTcpSocket* socket, auth::LoginAck ack);
    
    void SendLandingReq(QTcpSocket* socket);
    void HandleLandingAck(auth::LandingAck ack);
    
    void SendQuitReq(QTcpSocket* socket);
    void HandleQuitAck(auth::QuitAck ack);

    
private:
    QQmlApplicationEngine* qml_engine_;
    QTcpSocket *tcp_client_;
    
    QString user_name_;
    QString password_;
    
    EnterState state_ = { LoginState_None };
    
    std::string token_;
    
    NetCodec net_codec_;
};

#endif // LOGINMANAGER_H
