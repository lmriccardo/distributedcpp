#ifndef _SENDER_HPP
#define _SENDER_HPP

#include <iostream>
#include <type_traits>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Communication/Message.hpp>

namespace Lib::Network
{
    class Sender
    {
        public:
            virtual bool sendTo(const std::string& ip, const unsigned short port, unsigned char* buff, const std::size_t n) = 0;
            virtual void closeSocket() = 0;
            virtual bool isSocketClosed() = 0;
            virtual Socket& getSocket() = 0;
            bool sendTo(const std::string& ip, const unsigned short port, Message& msg);
    };

    class TcpSender : public Sender
    {
        private:
            TcpSocket _socket; // The socket of the sender

        public:
            TcpSender(const std::string& ip, unsigned short port) : _socket(ip, port) {};
            TcpSender(const TcpSocket& socket) : _socket(socket) {};
            ~TcpSender()
            {
                // close the socket when the sender goes out of scope
                _socket.closeSocket();
            }

            bool sendTo(const std::string& ip, const unsigned short port, 
                unsigned char* buff, const std::size_t n);

            using Sender::sendTo;

            void closeSocket()
            {
                _socket.closeSocket();
            }

            bool isSocketClosed()
            {
                return _socket.isClosed();
            }

            void setTimeout(long int sec, long int usec);
            void setTimeout(long int sec);
            void disconnect();

            TcpSocket& getSocket() override
            {
                return _socket;
            }

            void flushSocketError()
            {
                _socket.flushSocketError();
            }
            
    };

    class UdpSender : public Sender
    {
        private:
            UdpSocket _socket; // The socket of the Udp Sender
        
        public:
            UdpSender(const std::string& ip, unsigned short port) : _socket(ip, port) {};
            UdpSender(const UdpSocket& socket) : _socket(socket) {};
            ~UdpSender()
            {
                // Close the socket when the sender goes out of scope
                _socket.closeSocket();
            }

            bool sendTo(
                const std::string& ip, const unsigned short port, 
                unsigned char* buff, const std::size_t n);

            using Sender::sendTo;

            void closeSocket()
            {
                _socket.closeSocket();
            }

            bool isSocketClosed()
            {
                return _socket.isClosed();
            }

            UdpSocket& getSocket() override 
            {
                return _socket;
            }

            void flushSocketError()
            {
                _socket.flushSocketError();
            }
    };
}

#endif