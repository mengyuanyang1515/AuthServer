#ifndef LANDING_MANAGER_H
#define LANDING_MANAGER_H

#include <QObject>
#include <string>
#include "../common/net_codec.h"
#include "../common/auth.pb.h"
#include "../common/common_def.h"

class QTcpSocket;
class QTcpServer;
class LandingManager : public QObject
{
    Q_OBJECT
public:
    explicit LandingManager(QObject *parent = nullptr);
    void Initialize();

private slots:
    void newConnection();
    void disconnected();
    
    void clientConnected();
    void clientDisconnected();

private:
    void HandleMessage(QTcpSocket *socket, unsigned int message_id, const char* data, int len);
    
    void HandleLandingReq(QTcpSocket* socket, auth::LandingReq& req);
    void SendLandingAck(bool is_ok, const std::string& token);
    
    void HandleAuthAck(auth::AuthAck& ack);
    void SendAuthReq(QTcpSocket *socket);
        
    void HandleQuitReq(QTcpSocket* socket, auth::QuitReq& req);
    void SendQuitAck(QTcpSocket* socket);
    
private:
    QTcpServer *tcp_server_;
    QTcpSocket *tcp_client_;
    //QTcpSocket *client_;
    
    std::string token_;
    NetCodec net_codec_;
    
    std::map<std::string, QTcpSocket*> connected_clients_;
};

#endif // LANDING_MANAGER_H
