#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
using namespace boost::asio;
using namespace boost::asio::ip;
enum {
    MaxReadLength = 1024,
    MaxWriteLength = 1024
};

void handleConnect(std::function<void()> &byPassWrite, std::function<void()> &byPassRead, std::array<char, MaxReadLength> &dataToRead, std::array<char, MaxWriteLength> &dataToWrite,
                   tcp::socket &listeningSocket, io_context& ioContext, tcp::socket &connectingSocket,
                   tcp::resolver::results_type::iterator endpointsIter, const boost::system::error_code &ec)
{
    std::cout << "connected" << std::endl;


    byPassRead();
    byPassWrite();


    //ioContext.stop();
}

void handleResolve(std::function<void()>& byPassWrite, std::function<void()>& byPassRead, std::array<char, MaxReadLength> &dataToRead, std::array<char, MaxWriteLength> &dataToWrite,
                   tcp::socket &listeningSocket, io_context& ioContext,
                   tcp::socket &connectingSocket, tcp::resolver::results_type::iterator endpointsIter,
                   const boost::system::error_code &ec) {
    connectingSocket.async_connect(endpointsIter->endpoint(),
                                   [&byPassWrite, &byPassRead, &dataToRead, &dataToWrite, &listeningSocket, &ioContext, &connectingSocket, endpointsIter]
                                        (const boost::system::error_code& ec) {
                                       handleConnect(byPassWrite, byPassRead, dataToRead, dataToWrite, listeningSocket,
                                                     ioContext, connectingSocket, endpointsIter, ec);
                                   });
}

void handleAccept(std::function<void()> &byPassWrite, std::function<void()> &byPassRead, std::array<char, MaxReadLength> &dataToRead, std::array<char, MaxWriteLength> &dataToWrite,
                  tcp::socket& listeningSocket, tcp::resolver& resolver, io_context& ioContext,
                  tcp::socket& connectingSocket, const boost::system::error_code &ec)
{
    if (!ec) {
        //std::cout << __func__ << std::endl;

        resolver.async_resolve("192.168.16.2", "8554",
                               [&byPassWrite, &byPassRead, &dataToRead, &dataToWrite, &listeningSocket, &ioContext, &connectingSocket]
                                    (const boost::system::error_code &error, tcp::resolver::results_type results) {
            if (!error) {
                std::cout << "resolved count: " << results.size() << std::endl;
                auto endpointsIter = results.begin();
                if (endpointsIter != results.end()) {
                    handleResolve(byPassWrite, byPassRead, dataToRead, dataToWrite, listeningSocket, ioContext, connectingSocket, results, error);
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

int main()
{
    using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    io_context ioContext;
    work_guard_type work_guard(ioContext.get_executor());
    tcp::endpoint tcpEndPoint{tcp::v4(), 10554};
    std::cout << tcpEndPoint << std::endl;
    tcp::acceptor tcpAcceptor{ioContext, tcpEndPoint};
    tcp::socket listeningSocket{ioContext};
    tcp::socket connectingSocket{ioContext};
    tcp::resolver resolver{ioContext};

    std::array<char, MaxReadLength> dataToRead;
    std::array<char, MaxWriteLength> dataToWrite;

    std::function<void()> byPassRead;
    byPassRead = [&listeningSocket, &dataToRead, &connectingSocket, &byPassRead](){
        listeningSocket.async_read_some(boost::asio::buffer(dataToRead.data(), dataToRead.size()),
                                        [&dataToRead, &connectingSocket, &byPassRead](const boost::system::error_code &ec,
                                                                                      size_t ammountOfBytes) {
                                            if (!ec) {
                                                std::cout << ammountOfBytes << " bytes read" << std::endl;
                                                connectingSocket.async_write_some(
                                                        boost::asio::buffer(dataToRead.data(), ammountOfBytes),
                                                        [&byPassRead](const boost::system::error_code &ec, size_t ammountOfBytes) {
                                                            std::cout << ammountOfBytes << " bytes written"
                                                                      << std::endl;
                                                            byPassRead();
                                                        });
                                            } else {
                                                std::cerr << ec.message();
                                            }
                                        });

    };
    std::function<void()> byPassWrite;
    byPassWrite = [&listeningSocket, &dataToWrite, &connectingSocket, &byPassWrite]() {
        connectingSocket.async_read_some(boost::asio::buffer(dataToWrite.data(), dataToWrite.size()),
                                         [&dataToWrite, &listeningSocket, &byPassWrite](
                                                 const boost::system::error_code &ec,
                                                 size_t ammountOfBytes) {
                                             if (!ec) {
                                                 std::cout << ammountOfBytes << " bytes read" << std::endl;
                                                 listeningSocket.async_write_some(
                                                         boost::asio::buffer(dataToWrite.data(), ammountOfBytes),
                                                         [&byPassWrite](const boost::system::error_code &ec,
                                                                        size_t ammountOfBytes) {
                                                             std::cout << ammountOfBytes << " bytes written"
                                                                       << std::endl;
                                                             byPassWrite();
                                                         });
                                             } else {
                                                 std::cerr << ec.message();
                                             }
                                         });
    };

    tcpAcceptor.listen();
    tcpAcceptor.async_accept(listeningSocket, [&byPassWrite, &byPassRead, &dataToRead, &dataToWrite, &listeningSocket, &resolver, &ioContext, &connectingSocket](const boost::system::error_code &ec){
        handleAccept(byPassWrite, byPassRead, dataToRead, dataToWrite, listeningSocket, resolver, ioContext, connectingSocket, ec);
    });
    ioContext.run();
    return 0;
}
