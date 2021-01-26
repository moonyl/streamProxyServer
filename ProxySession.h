//
// Created by moony on 21. 1. 25..
//

#pragma once
#include <boost/asio/ip/tcp.hpp>


class ProxySession
{
public:
    enum {
        MaxReadLength = 1024,
        MaxWriteLength = 1024
    };

private:
    boost::asio::ip::tcp::socket reqSocket;
    boost::asio::ip::tcp::socket respSocket;
    std::array<char, MaxReadLength> reqData;
    std::array<char, MaxWriteLength> respData;

public:
    explicit ProxySession(boost::asio::io_context& ioContext);

    boost::asio::ip::tcp::socket& requestSocket();
    boost::asio::ip::tcp::socket& responseSocket();

    void readByPass();
    void writeByPass();

private:
    void onReceivedFromClient(const boost::system::error_code &ec, size_t ammountOfBytes);
    void onReceivedFromServer(const boost::system::error_code &ec, size_t ammountOfBytes);

    void onSentToClient(const boost::system::error_code &ec, size_t ammountOfBytes);

    void onSentToServer(const boost::system::error_code &ec, size_t ammountOfBytes);
};




