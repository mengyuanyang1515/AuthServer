#include "net_codec.h"
#include <QTcpSocket>
#include <QtEndian>

const static int kReadSocketLen = 65535;
const static int kHeaderLen = sizeof(short);
const static int kMessageIdLen = sizeof(unsigned int);

NetCodec::NetCodec(QObject *parent) : QObject(parent)
{
    
}

void NetCodec::RegisterSocket(QTcpSocket *socket)
{
    auto it = tcp_socket_info_.find(socket);
    if (it == tcp_socket_info_.end())
    {
        tcp_socket_info_.insert({socket, TcpSocketInfo()});
    }
}

void NetCodec::UnRegisterSocket(QTcpSocket *socket)
{
    auto it = tcp_socket_info_.find(socket);
    if (it != tcp_socket_info_.end())
    {
        qDebug() << "NetCodec::UnRegisterSocket 删除socket找到, socket:" << socket;
    }
    else
    {
        qDebug() << "NetCodec::UnRegisterSocket 删除socket没有找到, socket:" << socket;
    }
    tcp_socket_info_.erase(socket);
}

void NetCodec::SetMessageCallback(MessageCallback message_callback)
{
    message_callback_ = message_callback;
}


// 粘包逻辑  
void NetCodec::ReadyRead()
{
    qDebug() << "NetCodec调用ReadyRead3";
    qDebug() << "NetCodec调用ReadyRead3.1";
    
    QTcpSocket *socket = (QTcpSocket*)sender();
    qDebug() << "NetCodec调用ReadyRead3.2 socket:" << socket;
    auto it = tcp_socket_info_.find(socket);
    if (it != tcp_socket_info_.end())
    {
        qDebug() << "NetCodec::ReadyRead 在map找到了";
        TcpSocketInfo& tcp_socket_info = it->second;
        tcp_socket_info.byte_array_.append(socket->read(kReadSocketLen));
        qDebug() << "NetCodec::ReadyRead byte_array_.size:" << tcp_socket_info.byte_array_.size();
        if (tcp_socket_info.byte_array_.size() >= kHeaderLen + kMessageIdLen) // 2个字节的长度 和 4个字节的 message_id
        {
            // 需要处理大小端数据
            short len = qFromBigEndian<short>((uchar*)tcp_socket_info.byte_array_.left(kHeaderLen).data());
            qDebug() << "kHeaderLen:" << kHeaderLen << " kMessageIdLen:" << kMessageIdLen << " 粘包 len:" << len;
            if (tcp_socket_info.byte_array_.size() >= (len + kHeaderLen))
            {
                unsigned int msg_id = qFromBigEndian<unsigned int>((uchar*)tcp_socket_info.byte_array_.mid(kHeaderLen, kMessageIdLen).data());
                QByteArray data = tcp_socket_info.byte_array_.mid(kHeaderLen + kMessageIdLen, len - kMessageIdLen);
                
                if (message_callback_ != nullptr)
                {
                    message_callback_(socket, msg_id, data.constData(), len - kMessageIdLen);
                }
                
                tcp_socket_info.byte_array_.remove(0, kHeaderLen + len);
            }
        }
    }
    else {
        qDebug() << "NetCodec::ReadyRead 在map找不到";
    }
}

void NetCodec::WriteMessage(unsigned int message_id, const char *data, int len, QTcpSocket* socket)
{
    if (socket == nullptr)
    {
        return;
    }
    
    short messageLen_big = qToBigEndian<short>(len + kMessageIdLen);
    socket->write((const char*)(&messageLen_big), sizeof(messageLen_big));
    unsigned int msg_id_big = qToBigEndian<unsigned int>(message_id);
    socket->write((const char*)(&msg_id_big), sizeof(msg_id_big));
    socket->write(data);
}








