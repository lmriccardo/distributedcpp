#ifndef _QUBEINTERFACE_H
#define _QUBEINTERFACE_H

#include <iostream>
#include <string>
#include <memory>
#include <CommonLib/Communication/Interface.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <Configuration/Configuration.hpp>
#include <Logging/DisqubeLogger.hpp>
#include <Logging/ProgressBar.hpp>

namespace Qube
{
    class QubeMessageReceiver : public Lib::Concurrency::Thread
    {
    private:
        Lib::Concurrency::Queue_ptr<Lib::Network::ReceivedData> _queue; // The queue with all the messages
        Lib::Network::UdpCommunicationInterface_ptr _udpitf;            // Udp Communication Interface
        Lib::Network::TcpCommunicationInterface_ptr _tcpitf;            // Tcp Communication Interface
        bool _sigstop = false;

        void getFromUdpInterface(); // Receives a message from the UDP interface
        void getFromTcpInterface(); // Receives a message from the TCP interface

    public:
        QubeMessageReceiver(const Lib::Network::UdpCommunicationInterface_ptr &udpitf,
                            const Lib::Network::TcpCommunicationInterface_ptr &tcpitf)
            : Thread("Queue Message Dispatcher"), _udpitf(udpitf), _tcpitf(tcpitf)
        {
            _queue = std::make_shared<Lib::Concurrency::Queue<Lib::Network::ReceivedData>>(100);
        }

        void run() override;
        bool isRunning() const override;
        void stop();

        Lib::Network::ReceivedData getReceivedData();
        const std::size_t getCurrentQueueSize() const;
    };

    typedef std::shared_ptr<QubeMessageReceiver> QubeMessageReceiver_ptr;

    /**
     * @class Qube::QubeInterface
     *
     * Each cube is composed of a configuration and two communication interfaces:
     * an UDP Communication Interface and a TCP Interface. The UDP Communication
     * is used for those messages that do not require any layer of reliability or
     * that needs to be sent as fast as possible with no overhead due to re-transmission
     * and integrity check mechanism.
     *
     * Finally, a cube also contains a message dispatcher that every time dequeue
     * a message from one of the two queues and perform releated operations.
     *
     * A Qube can take up to 2 role: master or worker. A master qube is the one
     * dispatching the workload between multiple workers. There can only be one
     * master while all the others needs to be workers. The main difference between
     * these two roles is the kind of messages they can receive and send. For
     * example, a master qube can never receive a job posting message, as well
     * as a worker qube cannot send job posting requests.
     */
    class QubeInterface
    {
    private:
        Configuration::DisqubeConfiguration_ptr _conf;       // General configuration
        Lib::Network::UdpCommunicationInterface_ptr _udpitf; // Udp Communication Interface
        Lib::Network::TcpCommunicationInterface_ptr _tcpitf; // Tcp Communication Interface
        QubeMessageReceiver_ptr _receiver;                   // Qube message receiver
        Logging::DisqubeLogger_ptr _logger;                  // Generic logging class
        bool _isMaster;                                      // Master Qube interface or not.

        // A list of pair (ip, port no) for each node connected.
        std::vector<std::pair<std::string, unsigned short>> _nodes;

        void initUdpInterface(const std::string &ip);
        void initTcpInterface(const std::string &ip);
        void logInit();
        void init();

    public:
        QubeInterface(Configuration::DisqubeConfiguration_ptr conf, Logging::DisqubeLogger_ptr &logger)
            : _logger(logger)
        {
            _conf = conf;
            init(); // Call the init method
        }

        bool isMaster();                 // Check if the current interface is for a Master Qube
        void start();                    // Starts both UDP and TCP communication interface
        void stop();                     // Stops both UDP and TCP communication interface
        void qubeDiscovering();          // Performs the Discover protocol
        void interfaceDiagnosticCheck(); // Performs a check on TCP and UDP Interface

        Lib::Network::DiagnosticCheckResult *getUdpDiagnosticResult(); // Obtain result from UDP
        Lib::Network::DiagnosticCheckResult *getTcpDiagnosticResult(); // Obtain result from TCP

        void receiveAllMessage(); // Receive and handle all messages
        void processMessage(Lib::Network::ReceivedData& recvData); // Process the received message
    };

    typedef std::shared_ptr<QubeInterface> QubeInterface_ptr;
}

#endif