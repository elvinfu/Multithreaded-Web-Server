/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

using std::cerr;
using std::endl;
using std::string;
using std::to_string;

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int* const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:

  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = ai_family;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  // Get address info for given port and hints
  struct addrinfo* result;
  int res = getaddrinfo(nullptr, to_string(port_).c_str(), &hints, &result);
  if (res != 0) {
    cerr << "getaddrinfo error: " << gai_strerror(res) << endl;
    return false;
  }

  // Iterate through all the results and bind to the first available
  struct addrinfo* rp;
  for (rp = result; rp != nullptr; rp = rp->ai_next) {
    *listen_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (*listen_fd == -1) {
      continue;
    }

    if (bind(*listen_fd, rp->ai_addr, rp->ai_addrlen) == -1) {
      close(*listen_fd);
      continue;
    }

    break;  // Successfully bound to a socket
  }

  if (rp == nullptr) {
    cerr << "Failed: Did not bind to socket" << endl;
    return false;
  }
  freeaddrinfo(result);

  // Start listening for connections
  if (listen(*listen_fd, SOMAXCONN) == -1) {
    cerr << "Failed to mark socket as listening: " << strerror(errno) << endl;
    close(*listen_fd);
    return false;
  }

  listen_sock_fd_ = *listen_fd;
  return true;
}

bool ServerSocket::Accept(int* const accepted_fd,
                          std::string* const client_addr,
                          uint16_t* const client_port,
                          std::string* const client_dns_name,
                          std::string* const server_addr,
                          std::string* const server_dns_name) const {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:

  // Accept new conenction
  struct sockaddr_storage c_addr_info;
  socklen_t c_addr_len = sizeof(c_addr_info);
  *accepted_fd = accept(listen_sock_fd_,
    reinterpret_cast<struct sockaddr*>(&c_addr_info), &c_addr_len);
  if (*accepted_fd == -1) {
    cerr << "Failed to accept: " << strerror(errno) << endl;
    return false;
  }

  // Get client ip and port
  if (c_addr_info.ss_family == AF_INET) {
    char cp[INET_ADDRSTRLEN];
    struct sockaddr_in* in4_addr =
      reinterpret_cast<struct sockaddr_in*>(&c_addr_info);
    inet_ntop(AF_INET, &(in4_addr->sin_addr), cp, INET_ADDRSTRLEN);
    *client_port = ntohs(in4_addr->sin_port);
    *client_addr = cp;
  } else {
    char cp[INET6_ADDRSTRLEN];
    struct sockaddr_in6* in6_addr =
      reinterpret_cast<struct sockaddr_in6*>(&c_addr_info);
    inet_ntop(AF_INET6, &(in6_addr->sin6_addr), cp, INET6_ADDRSTRLEN);
    *client_port = ntohs(in6_addr->sin6_port);
    *client_addr = cp;
  }

  // Get client name info
  char c_hostname[NI_MAXHOST];
  Verify333(getnameinfo(reinterpret_cast<struct sockaddr*>(&c_addr_info),
    c_addr_len, c_hostname, NI_MAXHOST, nullptr, 0, 0) == 0);
  *client_dns_name = c_hostname;

  // Get server address
  char s_ip[INET6_ADDRSTRLEN];
  struct sockaddr_in6 local_addr;
  socklen_t local_addr_len = sizeof(local_addr);
  if (getsockname(*accepted_fd, reinterpret_cast<struct sockaddr*>(&local_addr),
    &local_addr_len) == -1) {
    cerr << "getsockname error: " << strerror(errno) << endl;
    return false;
  }
  inet_ntop(AF_INET6, &(local_addr.sin6_addr), s_ip, INET6_ADDRSTRLEN);
  *server_addr = s_ip;

  char s_hostname[NI_MAXHOST];

  Verify333(getnameinfo(reinterpret_cast<struct sockaddr*>(&local_addr),
    local_addr_len, s_hostname, NI_MAXHOST, nullptr, 0, 0) == 0);
  *server_dns_name = s_hostname;

  return true;
}

}  // namespace hw4
