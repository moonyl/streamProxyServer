#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include "ProxySession.h"
#include "ProxyAcceptor.h"

using namespace boost::asio;
using namespace boost::asio::ip;

int main()
{
    using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    io_context ioContext;
    work_guard_type work_guard(ioContext.get_executor());

    ProxyAcceptor acceptor{ioContext};

    acceptor.startAccept();

    ioContext.run();
    return 0;
}
