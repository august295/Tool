#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QTcpSocket>

#include "tcp_client_export.hpp"

class TCP_CLIENT_API TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);
    ~TcpClient();

    bool connect_server(const QString& host, int port);
    void disconnect_server();
    void send_message_string(const QByteArray& data);

    /**
     * @brief 获取通信信息
     * @return std::shared_ptr<QTcpSocket>
     */
    std::shared_ptr<QTcpSocket> get_tcp_socket();

private slots:
    void slot_ready_read();
    void slot_connected();
    void slot_disconnected();

signals:
    void signal_recv(const QByteArray& data);

private:
    std::shared_ptr<QTcpSocket> m_socket;
};

#endif // TCP_CLIENT_H
