#ifndef HTTPSERVERCONNECTION_H
#define HTTPSERVERCONNECTION_H

#include <mutex>
#include <queue>

#include "boost.h"

class BoostServer;
class HttpServerConnection
{
public:
    HttpServerConnection(BoostServer* server, tcp::socket& socket);
    ~HttpServerConnection();

public:
    void run();

    std::string getSession();
    typedef void (*DisconnectionCallback)(void*, std::string); //(server, session)
    void setDisconnectionCallback(DisconnectionCallback cb, void* arg)
    {
        m_disconnectionCallback = cb;
        m_arg                   = arg;
    }

private:
    std::string  mSession;
    BoostServer* mServer;
    tcp::socket  mSocket;

    DisconnectionCallback m_disconnectionCallback = nullptr;
    void*                 m_arg                   = nullptr;

    net::steady_timer                  mTimer{mSocket.get_executor(), std::chrono::seconds(10)};
    beast::flat_buffer                 mTempBuffer{8192};
    http::request<http::dynamic_body>  mHttpRequest;
    http::response<http::dynamic_body> mHttpResponse;

    FILE* fp = nullptr;
    void  handle();
    void  keepWrite();
};

#endif // HTTPSERVERCONNECTION_H