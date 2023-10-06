#ifndef NETCODEC_H
#define NETCODEC_H

#include <QObject>
#include <QByteArray>
#include <map>

class QTcpSocket;
typedef std::function<void (QTcpSocket* socket, unsigned int message_id, const char* data, int len)> MessageCallback;

struct TcpSocketInfo
{
    QByteArray byte_array_;
};

class NetCodec : public QObject
{
    Q_OBJECT
public:
    NetCodec(QObject *parent);
    
    void RegisterSocket(QTcpSocket* socket);
    void UnRegisterSocket(QTcpSocket* socket);
    void SetMessageCallback(MessageCallback message_callback);
    
    void ReadyRead();
    void WriteMessage(unsigned int message_id, const char* data, int len, QTcpSocket* socket = nullptr);
    
private:
    std::map<QTcpSocket*, TcpSocketInfo> tcp_socket_info_;
    MessageCallback message_callback_; // 粘包结束后的消息处理回调函数
};

#endif // NETCODEC_H
