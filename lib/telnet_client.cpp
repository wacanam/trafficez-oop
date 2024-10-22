#include "telnet_client.h"


TelnetClient::TelnetClient(const std::string &ip, int port)
    : relayIP_(ip), port_(port), sock_(-1) {
  if (relayIP_.empty()) {
    std::cerr << "Relay IP address is empty!" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void TelnetClient::sendCommand(const std::string &command) {
  // Automatically append "\r\n" to the command
  std::string fullCommand = command + "\r\n";
  fprintf(stdout, "Sending command: %s", fullCommand.c_str());

  ssize_t bytesSent = write(sock_, fullCommand.c_str(), fullCommand.length());
  if (bytesSent < 0) {
    perror("Failed to send command, attempting to reconnect...");
    // if (reconnect()) {
    sendCommand(command); // Retry sending the command after reconnecting
    // } else {
    //   std::cerr << "Reconnection failed." << std::endl;
    // }
  }
}

bool TelnetClient::authenticate(const std::string &username, const std::string &password) {
  std::string response;
  sendCommand(username);
  sendCommand("��-" + password);
  response = receiveResponse();

  if (response.find(">") != std::string::npos ||
      response.find("success") != std::string::npos) {
    return true;
  }

  return false;
}

// Helper function to wait for data to be available to read from the socket
bool waitForData(int sock, int timeoutSeconds) {
  fd_set readfds;
  struct timeval timeout;

  FD_ZERO(&readfds);
  FD_SET(sock, &readfds);

  timeout.tv_sec = timeoutSeconds;
  timeout.tv_usec = 0;

  int result = select(sock + 1, &readfds, NULL, NULL, &timeout);
  if (result > 0 && FD_ISSET(sock, &readfds)) {
    return true; // Data is available to read
  }
  return false; // No data available within the timeout
}

bool TelnetClient::connectClient(const std::string &username,
                                 const std::string &password) {

  struct sockaddr_in server_addr;
  sock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_ < 0) {
    perror("Socket creation failed");
    return false;
  }

  // Set socket timeout for receiving data
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) <
      0) {
    std::cerr << "Error setting socket options: " << strerror(errno)
              << std::endl;
    close(sock_);
    return false;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_);
  server_addr.sin_addr.s_addr = inet_addr(relayIP_.c_str());

  if (connect(sock_, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Connection to relay failed");
    close(sock_);
    return false;
  }
  // Set non-blocking mode for efficient input/output
  int flags = fcntl(sock_, F_GETFL, 0);
  if (flags == -1) {
    std::cerr << "Error getting socket flags: " << strerror(errno) << std::endl;
    return false;
  }

  flags |= O_NONBLOCK;
  if (fcntl(sock_, F_SETFL, flags) == -1) {
    std::cerr << "Error setting socket non-blocking: " << strerror(errno)
              << std::endl;
    return false;
  }

  if (!authenticate(username, password)) {
    std::cerr << "Failed to authenticate with relay module!" << std::endl;
    exit(EXIT_FAILURE);
  }
  return true;
}

std::string TelnetClient::receiveResponse(int retries, int timeoutSeconds) {
  std::string fullResponse;
  char buffer[1024];

  for (int i = 0; i < retries; ++i) {
    if (waitForData(sock_, timeoutSeconds)) {
      ssize_t bytesRead = read(sock_, buffer, sizeof(buffer) - 1);
      if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        fullResponse += buffer;   // Append to the full response
      }
    } else {
      std::cerr << "Timeout waiting for data..." << std::endl;
    }
  }

  std::cout << "Full response received: " << fullResponse
            << std::endl; // Debugging output
  return fullResponse;
}

std::string TelnetClient::getRelayStatus() {
  sendCommand("relay readall");
  return receiveResponse();
}

void TelnetClient::controlTrafficLights() {
  std::string statusHex = getRelayStatus();
  std::cout << "Relay Status (Hex): " << statusHex << std::endl;

  int relayStatus = std::stoi(statusHex, nullptr, 16);

  try {
    int relayStatus = std::stoi(statusHex, nullptr, 16);

    // Check the status of each relay (0-15)
    for (int i = 0; i < 16; ++i) {
      bool relayOn = relayStatus & (1 << i);
      std::cout << "Relay " << (i + 1) << " is " << (relayOn ? "ON" : "OFF")
                << std::endl;
    }
  } catch (const std::invalid_argument &e) {
    std::cerr << "Invalid relay status format: " << statusHex << std::endl;
  } catch (const std::out_of_range &e) {
    std::cerr << "Relay status out of range: " << statusHex << std::endl;
  }
}

bool TelnetClient::reconnect(const std::string &username,
                             const std::string &password) {
  close(sock_);
  return connectClient(username, password);
}

TelnetClient::~TelnetClient() {
  if (sock_ >= 0) {
    close(sock_);
  }
}