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

    bool connect_server(const QString& host, int port);
    void disconnect_server();
    void send_message_string(const QByteArray& data);

private slots:
    void slot_ready_read();
    void slot_connected();
    void slot_disconnected();

signals:
    void signal_recv(QString buff);

private:
    QTcpSocket* m_socket;
};

#endif // TCP_CLIENT_H
