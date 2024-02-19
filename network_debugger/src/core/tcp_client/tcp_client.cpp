#include "tcp_client.h"

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    m_socket->setProxy(QNetworkProxy::NoProxy);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::slot_ready_read);
    connect(m_socket, &QTcpSocket::connected, this, &TcpClient::slot_connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::slot_disconnected);
}

bool TcpClient::connect_server(const QString& host, int port)
{
    m_socket->connectToHost(host, port);
    return m_socket->waitForConnected(1000);
}

void TcpClient::disconnect_server()
{
    m_socket->disconnectFromHost();
}

void TcpClient::send_message_string(const QByteArray& data)
{
    m_socket->write(data);
    m_socket->flush();
}

void TcpClient::slot_ready_read()
{
    QByteArray data = m_socket->readAll();
    emit       signal_recv(data);
}

void TcpClient::slot_connected()
{
}

void TcpClient::slot_disconnected()
{
}
