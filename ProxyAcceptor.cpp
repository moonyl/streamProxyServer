//
// Created by moony on 21. 1. 26..
//

#include "ProxyAcceptor.h"
#include <iostream>

using namespace boost::asio::ip;

ProxyAcceptor::ProxyAcceptor(boost::asio::io_context &ioContext) :
        tcpEndPoint{tcp::v4(), 10554},
        tcpAcceptor{ioContext, tcpEndPoint},
        resolver{ioContext},
        ioContext{ioContext}
{
    tcpAcceptor.listen();
}

void ProxyAcceptor::onConnected(std::shared_ptr<ProxySession> session, tcp::resolver::results_type::iterator endpointsIter,
                                const boost::system::error_code &ec)
{
    std::cout << "connected" << std::endl;

    session->readByPass();
    session->writeByPass();
}

void ProxyAcceptor::onResolved(std::shared_ptr<ProxySession> session, tcp::resolver::results_type::iterator endpointsIter,
                               const boost::system::error_code &ec)
{
    session->responseSocket().async_connect(endpointsIter->endpoint(),
                                            [&, session, endpointsIter]
                                                    (const boost::system::error_code& ec) {
                                                onConnected(session, endpointsIter, ec);
                                            });
}


void ProxyAcceptor::onAccepted(std::shared_ptr<ProxySession> session, const boost::system::error_code &ec)    {
    if (!ec) {
        resolver.async_resolve("192.168.16.2", "8554",
                               [&, session](const boost::system::error_code &error,
                                            tcp::resolver::results_type results) {
                                   if (!error) {
                                       std::cout << "resolved count: " << results.size() << std::endl;
                                       auto endpointsIter = results.begin();
                                       if (endpointsIter != results.end()) {
                                           onResolved(session, results, error);
                                       } else {
                                           std::cerr << "cannot resolve" << std::endl;
                                           // TODO : should handle "close"
                                           return;
                                       }
                                   } else {
                                       std::cerr << error.message() << std::endl;
                                       return;
                                   }
                               });
    }
}

void ProxyAcceptor::startAccept() {
    auto session = std::make_shared<ProxySession>(ioContext);
    sessions.insert(session);
    tcpAcceptor.async_accept(session->requestSocket(), [&, session]
            (const boost::system::error_code &ec) {
        onAccepted(session, ec);
        //std::make_shared<ProxySession>(ioContext);
        startAccept();
    });
}
