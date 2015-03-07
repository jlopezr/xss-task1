#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "common.h"

using namespace std;
using boost::asio::ip::tcp;

int main(int argc, char** argv) {

    try
    {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), PORT));

        cout << "Server listening on port " << PORT << endl;
        
        for (;;)
        {
            tcp::socket socket(io_service);
            
            cout << "Waiting connection...\n";
            acceptor.accept(socket);
        
            boost::asio::ip::tcp::endpoint remote_ep = socket.remote_endpoint();
            cout << "Endpoint: " << remote_ep << endl;
            boost::asio::ip::address remote_ad = remote_ep.address();
            cout << "Address: " << remote_ad.to_string() << endl;

            std::string message = "HELLO";

            for(;;) {
                boost::system::error_code error;
                //cout << "Sending HELLO...\n";
                boost::asio::write(socket, boost::asio::buffer(message), error);

                if(error == boost::asio::error::connection_reset || error == boost::asio::error::broken_pipe) {
                    cout << "ERROR: " << error.message() << " (" << error.value() << ")" << endl;
                    cout << "Client disconnected" << endl;
                    break;
                }
            }
        
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
