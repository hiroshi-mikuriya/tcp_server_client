#include <arpa/inet.h> // to use inet_addr
#include <errno.h>
#include <thread>
#include <unistd.h> // to use close, write

#define PORT 12345

class Socket {
  int fd_;

public:
  Socket() : fd_(socket(AF_INET, SOCK_DGRAM, 0)) {
    if (fd_ < 0) {
      std::string msg = strerror(errno);
      throw std::runtime_error(("failed socket " + msg).c_str());
    }
  }

  ~Socket() {
    if (close(fd_) < 0) {
      perror("close");
    }
  }

  int get() const { return fd_; }
};

void udpServer() {
  Socket sock;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    perror("server bind");
    return;
  }
  char buf[256] = {0};
  if (recv(sock.get(), buf, sizeof(buf), 0) < 0) {
    perror("server recv");
    return;
  }
  puts(buf);
}

void udpClient() {
  Socket sock;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  char msg[] = "hello world";
  if (sendto(sock.get(), msg, sizeof(msg), 0,
             reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    perror("client sendto");
    return;
  }
}

int main() {
  std::thread th(udpServer);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  udpClient();
  th.join();
  return 0;
}
