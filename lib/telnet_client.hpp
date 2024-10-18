#include <iostream>
#include <string>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <functional>
#include <thread>

class TelnetClient
{
public:
    TelnetClient(const std::string &hostname, int port) : hostname_(hostname), port_(port), sockfd_(0) {}

    bool connectClient()
    {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ == -1)
        {
            std::cerr << "Error creating socket: " << gai_strerror(errno) << std::endl;
            return false;
        }

        struct hostent *host = gethostbyname(hostname_.c_str());
        if (host == NULL)
        {
            std::cerr << "Error resolving hostname: " << gai_strerror(errno) << std::endl;
            return false;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port_);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

        if (connect(sockfd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            std::cerr << "Error connecting to server: " << gai_strerror(errno) << std::endl;
            return false;
        }

        // Set non-blocking mode for efficient input/output
        int flags = fcntl(sockfd_, F_GETFL, 0);
        if (flags == -1)
        {
            std::cerr << "Error getting socket flags: " << gai_strerror(errno) << std::endl;
            return false;
        }

        flags |= O_NONBLOCK;
        if (fcntl(sockfd_, F_SETFL, flags) == -1)
        {
            std::cerr << "Error setting socket non-blocking: " << gai_strerror(errno) << std::endl;
            return false;
        }

        return true;
    }

    void sendCommand(const std::string &command, std::function<void(const std::string &)> callback)
    {
        std::thread thread([this, command, callback]()
                           {
            int bytes_sent = send(sockfd_, command.c_str(), command.length(), 0);
            if (bytes_sent == -1) {
                std::cerr << "Error sending command: " << gai_strerror(errno)  << std::endl;
                return;
            }

            char buffer[1024];
            std::string response;
            while (true) {
                int bytes_received = recv(sockfd_, buffer, sizeof(buffer), 0);
                if (bytes_received == -1) {
                    std::cerr << "Error receiving data: " << gai_strerror(errno)  << std::endl;
                    break;
                }

                if (bytes_received == 0) {
                    break;
                }

                response.append(buffer, bytes_received);
            }

            callback(response); });

        thread.detach();
    }

    ~TelnetClient()
    {
        if (sockfd_ != 0)
        {
            close(sockfd_);
        }
    }

private:
    std::string hostname_;
    int port_;
    int sockfd_;
};
