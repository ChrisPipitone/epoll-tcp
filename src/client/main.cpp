#include "common/AddrInfo.h"
#include "common/Fd.h"
#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <system_error>

Fd connect_to(const char* host_name, const char* port) {
  int last_err = 0;
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  auto addr_info = resolve(host_name, port, hints);
  for (auto* p = addr_info.get(); p != nullptr; p = p->ai_next) {
    Fd client_fd(socket(p->ai_family, p->ai_socktype, p->ai_protocol));
    if (client_fd.get() >= 0 &&
        connect(client_fd.get(), p->ai_addr, p->ai_addrlen) == 0)
      return client_fd;
    last_err = errno;
  }
  throw std::system_error(last_err, std::generic_category(), "connect");
}

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "usage: client <host> <port>\n";
    return 1;
  }

  const char* kHostName = argv[1];
  const char* kPort = argv[2];

  try {
    Fd connection_fd = connect_to(kHostName, kPort);

    // sending data
    std::string message;
    while (std::getline(std::cin, message) && message != "/quit") {
      message += '\n';
      if (send(connection_fd.get(), message.data(), message.size(), 0) < 0)
        throw std::system_error(errno, std::generic_category(), "send");
    }

  } catch (const std::exception& e) {
    std::cerr << "fatal: " << e.what() << "\n";
    return 1;
  }
}
