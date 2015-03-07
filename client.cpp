#include <ctime>
#include <csignal>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include "common.h"

#define _CSIGNAL_

using namespace std;
using boost::asio::ip::tcp;

long totalLength;
boost::posix_time::ptime initial;

#ifdef _CSIGNAL_
void signal_handler(int signum) {
    cout << "[C Style] Signal " << signum << endl;
    
    auto end = boost::posix_time::microsec_clock::universal_time();
    auto result = end - initial;
    cout << "Time: " << result.total_microseconds() << " uS"<< endl;
    cout << "Length: " << totalLength << " bytes" << endl;
    
    double speed = ((double)totalLength / result.total_microseconds())*8000.0;
    cout << "Speed: " << speed << " Mbps" << endl;

    
    exit(signum);
}
#endif

#ifdef _BOOST_SIGNAL_
void signal_handler(
    const boost::system::error_code& error,
    int signal_number)
{
    if (!error)
    {
        cout << "[BOOST Style] Signal " << signal_number << endl;
    } else {
        cerr << "Error in signal handler: " << error << endl;
    }
}
#endif

int main(int argc, char** argv) {

#ifdef _CSIGNAL_
    cout << "Setting C style signal handler!" << endl;
    signal(SIGINT, signal_handler);
#endif

    //boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    initial = boost::posix_time::microsec_clock::universal_time();
    totalLength = 0;
    
    try {
        boost::asio::io_service io_service;

#ifdef _BOOST_SIGNAL_
        cout << "Setting BOOST style signal handler!" << endl;
        boost::asio::signal_set signals(io_service, SIGQUIT, SIGINT, SIGTERM);
        signals.async_wait(signal_handler);
#endif
        
        tcp::resolver resolver(io_service);
        tcp::resolver::query query("localhost", std::to_string(PORT));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        boost::posix_time::ptime start, end;
        long partialLength = 0;
      

        start = boost::posix_time::microsec_clock::universal_time();
        
        //for (;;)
        for (int i=0;i<1000000;i++)
        {
            boost::array<char, 1024> buf;
            boost::system::error_code error;

            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            
            //std::cout.write(buf.data(), len);
            
            totalLength += len;
            partialLength += len;
            
            end = boost::posix_time::microsec_clock::universal_time();
            auto result = end - start;
            
            if(result.total_seconds() == 1) {
                cout << "-------------------------------------" << endl;
                cout << "Time: " << result.total_microseconds() << " uS"<< endl;
                cout << "Length: " << partialLength << " bytes" << endl;
                
                double speed = ((double)partialLength / result.total_microseconds())*8000.0;
                cout << "Speed: " << speed << " Kbps" << endl;
                speed = ((double)partialLength / result.total_microseconds())*(1.0e+6/1024.0);
                cout << "Speed: " << speed << " kbytes/s" << endl;
                
                partialLength = 0;
                start = boost::posix_time::microsec_clock::universal_time();
            }
            
        }
        
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket.close();
        
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    auto end = boost::posix_time::microsec_clock::universal_time();
    auto result = end - initial;
    cout << "=====================================" << endl;
    cout << "Time: " << result.total_microseconds() << " uS"<< endl;
    cout << "Length: " << totalLength << " bytes" << endl;
    
    double speed = ((double)totalLength / result.total_microseconds())*8000.0;
    cout << "Speed: " << speed << " kbits/s" << endl;
    speed = ((double)totalLength / result.total_microseconds())*(1.0e+6/1024.0);
    cout << "Speed: " << speed << " kbytes/s" << endl;

    return 0;
}
