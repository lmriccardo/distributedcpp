#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <CommonLib/Communication/ByteBuffer.hpp>

namespace Lib::Network
{
    const unsigned short MAX_MESSAGE_CAPACITY = 65535;

    class Message : public ByteBuffer
    {
    public:
        enum class MessageProto
        {
            TCP, // Tcp Message Protocol
            UDP  // Udp Message Protocol
        };

        enum class MessageType
        {
            SIMPLE,  // Used only for simple string message
            DISCOVER // Used for the discover protocol
        };

        enum class MessageSubType
        {
            SIMPLE = 0,
            DISCOVER_HELLO = 1,   // A message usually sent from the master to workers
            DISCOVER_RESPONSE = 2 // A response message for the HELLO
        };

    protected:
        MessageType _type;
        MessageSubType _subType;
        MessageProto _proto;
        unsigned short _counter;
        unsigned short _id;
        uint8_t _flag;

        const static std::size_t NUM_HEAD_BYTES = 8;

        static void encode_(Message &msg);
        static void decode_(Message &msg);

    public:
        Message(const MessageType &type, const MessageSubType &subType,
                uint16_t id, uint16_t counter, const std::size_t nofBytes)
            : ByteBuffer(nofBytes), _type(type), _subType(subType),
              _counter(counter), _id(id) {}

        Message(const MessageType &type, const MessageSubType &subType, uint16_t id,
                uint16_t counter) : Message(type, subType, id, counter, MAX_MESSAGE_CAPACITY) {}

        Message(const unsigned char *buffer, const std::size_t nofBytes)
            : ByteBuffer(nofBytes)
        {
            put(buffer, nofBytes);
            position(0);
        }

        Message(const ByteBuffer &buffer) : ByteBuffer(buffer) {};

        ~Message() = default;

        const unsigned short getMessageCounter() const;
        const unsigned short getMessageId() const;
        const MessageType getMessageType() const;
        const MessageSubType getMessageSubType() const;
        const MessageProto getMessageProtocol() const;
        const unsigned int getMessageTypeId() const;
        const uint8_t getMessageProtoFlags() const;

        void setMessageType(const MessageType &type);
        void setMessageSubType(const MessageSubType &subType);
        void setMessageId(const unsigned short id);
        void setMessageCounter(const unsigned short counter);
        void setMessageProtocol(const MessageProto &proto);

        virtual void encode() = 0;
        virtual void decode() = 0;
    };

    // The data received from a socket will be put into a structure
    // divided into the ByteBuffer and the client information
    struct ReceivedData
    {
        ByteBuffer_ptr data;     // The ByteBuffer with the received bytes
        struct sockaddr_in *src; // Informations of the sender
    };

    class SimpleMessage : public Message
    {
    private:
        std::string _msg;

    public:
        SimpleMessage(const uint16_t id, const uint16_t counter, std::string &msg)
            : Message(
                  Message::MessageType::SIMPLE, Message::MessageSubType::SIMPLE, id, counter,
                  NUM_HEAD_BYTES + msg.size()),
              _msg(msg) {};

        SimpleMessage(const unsigned char *buff, const std::size_t nofBytes)
            : Message(buff, nofBytes)
        {
            decode();
        }

        SimpleMessage(const ByteBuffer &buffer) : Message(buffer)
        {
            decode();
        }

        ~SimpleMessage() = default;

        const std::string &getMessage() const;
        void encode();
        void decode();
    };

    class DiscoverHelloMessage : public Message
    {
    private:
        unsigned short _udpPort;
        unsigned short _tcpPort;

        static const std::size_t MSG_NUM_BYTES = 4;

    public:
        DiscoverHelloMessage(const uint16_t id, const uint16_t counter)
            : Message(MessageType::DISCOVER, MessageSubType::DISCOVER_HELLO,
                      id, counter, NUM_HEAD_BYTES + MSG_NUM_BYTES) {};

        DiscoverHelloMessage(const ByteBuffer &buffer) : Message(buffer)
        {
            decode();
        }

        void setUdpPort(const unsigned short udpPort);
        void setTcpPort(const unsigned short tcpPort);

        unsigned short getUdpPort() const;
        unsigned short getTcpPort() const;

        void encode();
        void decode();
    };

    class DiscoverResponseMessage : public Message
    {
    private:
        unsigned short _udpPort;
        unsigned short _tcpPort;
        uint8_t _memory;
        uint8_t _ncpus;
        unsigned short _qdim;

        static const std::size_t MSG_NUM_BYTES = 8;

    public:
        DiscoverResponseMessage(const uint16_t id, const uint16_t counter)
            : Message(MessageType::DISCOVER, MessageSubType::DISCOVER_RESPONSE,
                      id, counter, NUM_HEAD_BYTES + MSG_NUM_BYTES) {};

        DiscoverResponseMessage(const ByteBuffer &buffer) : Message(buffer)
        {
            decode();
        }

        void setUdpPort(const unsigned short udpPort);
        void setTcpPort(const unsigned short tcpPort);
        void setAvailableMemory(const uint8_t memory);
        void setNumberOfCpus(const uint8_t ncpus);
        void setTaskQueueDimension(unsigned short qdim);

        unsigned short getUdpPort() const;
        unsigned short getTcpPort() const;
        uint8_t getAvailableMemory() const;
        uint8_t getNumberOfCpus() const;
        unsigned short getTaskQueueDimension() const;

        void encode();
        void decode();
    };
}

#endif