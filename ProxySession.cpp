//
// Created by moony on 21. 1. 25..
//

#include "ProxySession.h"
#include <iostream>

ProxySession::ProxySession(boost::asio::io_context &ioContext) :
    reqSocket{ioContext}, respSocket{ioContext}
{

}

boost::asio::ip::tcp::socket &ProxySession::requestSocket()  {
    return reqSocket;
}

boost::asio::ip::tcp::socket &ProxySession::responseSocket()  {
    return respSocket;
}

void ProxySession::onSentToClient(const boost::system::error_code &ec, size_t ammountOfBytes)
{
//    std::cout << ammountOfBytes << " bytes written"
//              << std::endl;
    writeByPass();
}

void ProxySession::onReceivedFromServer(const boost::system::error_code &ec, size_t ammountOfBytes)
{
    if (!ec) {
//        std::cout << ammountOfBytes << " bytes read" << std::endl;
        reqSocket.async_write_some(
                boost::asio::buffer(respData.data(), ammountOfBytes),
                [&](const boost::system::error_code &ec, size_t ammountOfBytes) {
                    onSentToClient(ec, ammountOfBytes);
                });
    } else {
        std::cerr << ec.message();
    }
}

void ProxySession::writeByPass() {
    respSocket.async_read_some(boost::asio::buffer(respData.data(), respData.size()),
                                     [&](const boost::system::error_code &ec, size_t ammountOfBytes) {
                                         onReceivedFromServer(ec, ammountOfBytes);
                                     });
}

void ProxySession::onSentToServer(const boost::system::error_code &ec, size_t ammountOfBytes)
{
//    std::cout << ammountOfBytes << " bytes written"
//              << std::endl;
    if (ammountOfBytes == 0)    {
        std::cout << "why 0 byte written?" << std::endl;
        return;
    }
    readByPass();
}

void ProxySession::onReceivedFromClient(const boost::system::error_code &ec, size_t ammountOfBytes)
{
    if (!ec) {
//        std::cout << ammountOfBytes << " bytes read" << std::endl;
        respSocket.async_write_some(
                boost::asio::buffer(reqData.data(), ammountOfBytes),
                [&](const boost::system::error_code &ec, size_t ammountOfBytes) {
                    onSentToServer(ec, ammountOfBytes);
                });
    } else {
        std::cerr << ec.message();
    }
}

void ProxySession::readByPass() {
    reqSocket.async_read_some(boost::asio::buffer(reqData.data(), reqData.size()),
                                    [&](const boost::system::error_code &ec, size_t ammountOfBytes) {
                                        onReceivedFromClient(ec, ammountOfBytes);
                                    });
}
