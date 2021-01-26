//
// Created by moony on 21. 1. 26..
//


#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <boost/asio/ip/basic_resolver_results.hpp>
#include "ProxySession.h"

class ProxyAcceptor {
    boost::asio::ip::tcp::endpoint tcpEndPoint;
    boost::asio::ip::tcp::acceptor tcpAcceptor;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::io_context &ioContext;

    std::set<std::shared_ptr<ProxySession>> sessions;
public:
    explicit ProxyAcceptor(boost::asio::io_context& ioContext);

    void startAccept();

private:
    void onAccepted(std::shared_ptr<ProxySession> session, const boost::system::error_code &ec);

    void onResolved(std::shared_ptr<ProxySession> session,
               boost::asio::ip::tcp::resolver::results_type::iterator endpointsIter,
               const boost::system::error_code &ec);

    void onConnected(std::shared_ptr<ProxySession> session,
                boost::asio::ip::tcp::resolver::results_type::iterator endpointsIter,
                const boost::system::error_code &ec);
};




