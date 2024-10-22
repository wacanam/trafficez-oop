#ifndef TELNET_CLIENT_H
#define TELNET_CLIENT_H

#include <string>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h> // Add this include for fcntl
#include <sys/select.h>

class TelnetClient {
public:
  TelnetClient(const std::string &ip, int port);
  ~TelnetClient();
  bool connectClient(const std::string &username, const std::string &password);
  void sendCommand(const std::string &command);
  std::string getRelayStatus();
  void controlTrafficLights();
  bool authenticate(const std::string &username, const std::string &password);

protected:
  bool reconnect(const std::string &username, const std::string &password);

private:
  std::string receiveResponse(int retries = 5, int timeoutSeconds = 5);
  std::string relayIP_;
  int port_;
  int sock_;
};

#endif
