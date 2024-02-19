#include "tcp_client.h"

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
{
    m_socket = std::make_shared<QTcpSocket>();
    m_socket->setProxy(QNetworkProxy::NoProxy);
    connect(m_socket.get(), &QTcpSocket::readyRead, this, &TcpClient::slot_ready_read);
    connect(m_socket.get(), &QTcpSocket::connected, this, &TcpClient::slot_connected);
    connect(m_socket.get(), &QTcpSocket::disconnected, this, &TcpClient::slot_disconnected);
}

TcpClient::~TcpClient()
{
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

std::shared_ptr<QTcpSocket> TcpClient::get_tcp_socket()
{
    return m_socket;
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
