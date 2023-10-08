#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <QObject>
#include "../common/net_codec.h"
#include "../common/auth.pb.h"

class QTcpSocket; // 前置依赖，减少编译器依赖
class QTcpServer;
class AuthManager : public QObject
{
    Q_OBJECT
    
public:
    AuthManager(QObject *parent = 0);

    void Initialize();
    
private slots:
    void newConnection();
    void disconnected();

private:
    void HandleMessage(QTcpSocket* socket, unsigned int message_id, const char* data, int len);
    
    void HandleRegisterReq(QTcpSocket* socket, auth::RegisterReq& req);
    void SendRegisterAck(QTcpSocket* socket, bool is_ok, const std::string& error_info);
    
    void HandleLoginReq(QTcpSocket* socket, auth::LoginReq& req);
    void SendLoginAck(QTcpSocket* socket, bool is_ok, const std::string& name);
        
    void HandleAuthReq(QTcpSocket* socket, auth::AuthReq& req);
    void SendAuthAck(QTcpSocket* socket, bool is_ok, const std::string& token);
    
private:
    QTcpServer* tcp_server_;
    
    NetCodec net_codec_; // 生命周期完全由AuthManager控制
};

#endif // AUTH_MANAGER_H
