#ifndef BOOST_H
#define BOOST_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http  = boost::beast::http;   // from <boost/beast/http.hpp>
namespace net   = boost::asio;          // from <boost/asio.hpp>
using tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

#endif // BOOST_H
