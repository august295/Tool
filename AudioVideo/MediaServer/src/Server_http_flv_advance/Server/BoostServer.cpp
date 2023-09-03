#include "BoostServer.h"
#include "HttpServerConnection.h"

BoostServer::BoostServer(Config* config)
    : mConfig(config)
{
}
BoostServer::~BoostServer()
{
    if (mAcceptor)
    {
        delete mAcceptor;
        mAcceptor = nullptr;
    }
    if (mIoc)
    {
        delete mIoc;
        mIoc = nullptr;
    }
}
void BoostServer::start()
{
    int            threadNum = std::max<int>(1, mConfig->getThreadNum());
    unsigned short port      = mConfig->getPort();
    LOGI("BoostServer::start() ip=%s,port=%d,threadNum=%d", mConfig->getIp(), port, threadNum);

    boost::asio::ip::address address = net::ip::make_address(mConfig->getIp());
    tcp::endpoint            endpoint{address, port};

    mIoc      = new net::io_context{threadNum};
    mAcceptor = new tcp::acceptor(*mIoc);

    beast::error_code ec;
    mAcceptor->open(endpoint.protocol(), ec);
    if (ec)
    {
        LOGE("acceptor.open error: %s", ec.message().data());
        return;
    }
    mAcceptor->set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        LOGE("acceptor.set_option error: %s", ec.message().data());
        return;
    }
    mAcceptor->bind(endpoint, ec);
    if (ec)
    {
        LOGE("acceptor.bind error: %s", ec.message().data());
        return;
    }
    mAcceptor->listen(net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        LOGE("acceptor.listen error: %s", ec.message().data());
        return;
    }
    setOnAccept();

    std::vector<std::thread> ts;
    ts.reserve(threadNum - 1);

    for (auto i = 0; i < threadNum - 1; ++i)
    {
        ts.emplace_back([this] {
            std::thread::id threadId = std::this_thread::get_id();
            LOGI("ioc sub threadId=%lld", threadId);
            mIoc->run();
        });
    }
    std::thread::id threadId = std::this_thread::get_id();
    LOGI("ioc main threadId=%lld", threadId);
    mIoc->run();
}

void BoostServer::setOnAccept()
{
    mAcceptor->async_accept(net::make_strand(*mIoc), beast::bind_front_handler(&BoostServer::onAccept, this));
}
void BoostServer::onAccept(beast::error_code ec, tcp::socket socket)
{
    if (ec)
    {
        LOGE("onAccept error: %s", ec.message().data());
        return;
    }
    else
    {
        HttpServerConnection* conn = new HttpServerConnection(this, socket);
        if (this->addConn(conn))
        {
            conn->setDisconnectionCallback(BoostServer::cbDisconnection, this);
            conn->run();
        }
        else
        {
            delete conn;
            conn = nullptr;
        }
    }
    setOnAccept();
}

bool BoostServer::addConn(HttpServerConnection* conn)
{
    m_connMap_mtx.lock();
    if (m_connMap.find(conn->getSession()) != m_connMap.end())
    {
        m_connMap_mtx.unlock();
        return false;
    }
    else
    {
        m_connMap.insert(std::make_pair(conn->getSession(), conn));
        LOGI("[AddConn] On-Live Client Size=%lld", m_connMap.size());
        m_connMap_mtx.unlock();
        return true;
    }
}
bool BoostServer::removeConn(std::string session)
{
    m_connMap_mtx.lock();
    std::map<std::string, HttpServerConnection*>::iterator it = m_connMap.find(session);
    if (it != m_connMap.end())
    {
        m_connMap.erase(it);
		LOGI("[RemoveConn] On-Live Client Size=%lld", m_connMap.size());
        m_connMap_mtx.unlock();
        return true;
    }
    else
    {
        m_connMap_mtx.unlock();
        return false;
    }
}
HttpServerConnection* BoostServer::getConn(std::string session)
{
    m_connMap_mtx.lock();
    std::map<std::string, HttpServerConnection*>::iterator it = m_connMap.find(session);
    if (it != m_connMap.end())
    {
        m_connMap_mtx.unlock();
        return it->second;
    }
    else
    {
        m_connMap_mtx.unlock();
        return nullptr;
    }
}
void BoostServer::cbDisconnection(void* arg, std::string session)
{
    BoostServer* server = (BoostServer*)arg;
    server->handleDisconnection(session);
}
bool BoostServer::sendData(char* data, int size)
{
    bool result = false;

    m_connMap_mtx.lock();
    if (m_connMap.size() > 0)
    {
        result = true;
    }

    for (auto it = m_connMap.begin(); it != m_connMap.end(); it++)
    {
        HttpServerConnection* conn = it->second;
    }
    m_connMap_mtx.unlock();
    return result;
}

void BoostServer::handleDisconnection(std::string session)
{
    LOGI("session=%s,disconnection", session.data());
    HttpServerConnection* conn = this->getConn(session);
    this->removeConn(session);
    if (conn)
    {
        delete conn;
        conn = nullptr;
    }
}
std::string BoostServer::generateSession()
{
    std::string numStr;
    numStr.append(std::to_string(rand() % 9 + 1));
    numStr.append(std::to_string(rand() % 10));
    numStr.append(std::to_string(rand() % 10));
    numStr.append(std::to_string(rand() % 10));
    numStr.append(std::to_string(rand() % 10));
    numStr.append(std::to_string(rand() % 10));
    numStr.append(std::to_string(rand() % 10));
    numStr.append(std::to_string(rand() % 10));
    return numStr;
}

Config* BoostServer::GetConfig()
{
    return mConfig;
}
