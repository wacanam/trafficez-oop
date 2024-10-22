#include <arpa/inet.h>
#include <chrono>
#include <cstring> // Add this include for strerror
#include <fcntl.h> // Add this include for fcntl
#include <functional>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/select.h> // Add this include for select
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

class TelnetClient {
public:
  TelnetClient(const std::string &hostname, int port)
      : hostname_(hostname), port_(port), sockfd_(-1) {}

  bool connectClient(const std::string &username, const std::string &password,
                     int timeout_ms) {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
      std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
      return false;
    }

    // Set socket timeout for receiving data
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
      std::cerr << "Error setting socket options: " << strerror(errno) << std::endl;
      close(sockfd_);
      return false;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = inet_addr(hostname_.c_str());

    if (connect(sockfd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
      std::cerr << "Error connecting to server: " << strerror(errno) << std::endl;
      close(sockfd_);
      return false;
    }
    // // Set non-blocking mode for efficient input/output
    // int flags = fcntl(sockfd_, F_GETFL, 0);
    // if (flags == -1) {
    //   std::cerr << "Error getting socket flags: " << strerror(errno)
    //             << std::endl;
    //   return false;
    // }

    // flags |= O_NONBLOCK;
    // if (fcntl(sockfd_, F_SETFL, flags) == -1) {
    //   std::cerr << "Error setting socket non-blocking: " << strerror(errno)
    //             << std::endl;
    //   return false;
    // }

    // Send username and password immediately after connection
    sendCommand(
        username,
        [](const std::string &response) {
          std::cout << "Received response: " << response << std::endl;
        },
        timeout_ms);

    sendCommand(
        password,
        [](const std::string &response) {
          std::cout << "Received response: " << response << std::endl;
        },
        timeout_ms);

    // // Wait for authentication response with timeout
    // std::chrono::steady_clock::time_point start =
    //     std::chrono::steady_clock::now();
    // std::string response;
    // while (true) {
    //   fd_set read_fds;
    //   FD_ZERO(&read_fds);
    //   FD_SET(sockfd_, &read_fds);

    //   struct timeval tv;
    //   tv.tv_sec = 1; // Check every 1 second
    //   tv.tv_usec = 0;

    //   int retval = select(sockfd_ + 1, &read_fds, NULL, NULL, &tv);
    //   if (retval == -1) {
    //     std::cerr << "Error in select: " << strerror(errno) << std::endl;
    //     close(sockfd_); // Close the socket to clean up resources
    //     return false;
    //   } else if (retval == 0) {
    //     // Check for timeout
    //     std::chrono::steady_clock::time_point now =
    //         std::chrono::steady_clock::now();
    //     std::chrono::duration<double> elapsed_seconds = now - start;
    //     if (elapsed_seconds.count() * 1000 > timeout_ms) {
    //       std::cerr << "Timeout waiting for authentication response"
    //                 << std::endl;
    //       close(sockfd_); // Close the socket to clean up resources
    //       return false;
    //     }
    //     continue; // No data available, continue waiting
    //   }

    //   if (FD_ISSET(sockfd_, &read_fds)) {
    //     char buffer[1024];
    //     int bytes_received = read(sockfd_, buffer, sizeof(buffer));
    //     if (bytes_received == -1) {
    //       std::cerr << "Error receiving data: " << strerror(errno) <<
    //       std::endl; close(sockfd_); // Close the socket to clean up
    //       resources return false;
    //     }

    //     if (bytes_received == 0) {
    //       break;
    //     }

    //     response.append(buffer, bytes_received);
    //   }
    // }

    // Check if authentication was successful
    // if (response.find("Authentication successful") == std::string::npos) {
    //   std::cerr << "Authentication failed: " << response << std::endl;
    //   return false;
    // }

    return true;
  }

  void sendData(const std::string &data) {
    std::string full_data = data + "\r\n";
    int bytes_sent = send(sockfd_, full_data.c_str(), data.length(), 0);
    if (bytes_sent == -1) {
      std::cerr << "Error sending data: " << strerror(errno) << std::endl;
    }
    std::cout << "Data sent: " << data << std::endl;
  }

  void sendCommand(const std::string &command,
                   std::function<void(const std::string &)> callback,
                   int timeout_ms) {
    std::thread thread([this, command, callback, timeout_ms]() {
      sendData(command);

      // Create a separate thread to receive data asynchronously
      std::thread receiveThread([this, callback, timeout_ms]() {
        char buffer[1024];
        std::string response;
        std::chrono::steady_clock::time_point start =
            std::chrono::steady_clock::now();

        while (true) {
          int bytes_received = read(sockfd_, buffer, sizeof(buffer) -1);
          if (bytes_received == -1) {
            std::cerr << "Error receiving data: " << strerror(errno)
                      << std::endl;
            break;
          }

          if (bytes_received == 0) {
            break;
          }

          response.append(buffer, bytes_received);

          // Check for timeout
          std::chrono::steady_clock::time_point now =
              std::chrono::steady_clock::now();
          std::chrono::duration<double> elapsed_seconds = now - start;
          if (elapsed_seconds.count() * 1000 > timeout_ms) {
            std::cerr << "Timeout waiting for response" << std::endl;
            break;
          }
        }

        callback(response);
      });

      receiveThread.detach();
    });

    thread.detach();
  }

  ~TelnetClient() {
    if (sockfd_ != 0) {
      close(sockfd_);
    }
  }

private:
  std::string hostname_;
  int port_;
  int sockfd_;
};