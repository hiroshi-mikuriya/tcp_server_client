#include <arpa/inet.h> // to use inet_addr
#include <errno.h>
#include <thread>
#include <unistd.h> // to use close, write

#define PORT 12345

class Socket {
  int fd_;

public:
  Socket() : fd_(socket(AF_INET, SOCK_STREAM, 0)) {
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

void tcpServer() {
  Socket sock0;

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sock0.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) <
      0) {
    perror("server bind");
    return;
  }

  if (listen(sock0.get(), 5) < 0) {
    perror("server listen");
    return;
  }

  sockaddr_in client;
  socklen_t len = sizeof(client);
  int sock = accept(sock0.get(), reinterpret_cast<sockaddr *>(&client), &len);
  if (sock < 0) {
    perror("server listen");
    return;
  }

  char buf[256] = {0};
  if (read(sock, buf, sizeof(buf)) < 0) {
    perror("server read");
    return;
  }
  puts(buf);

  if (write(sock, "hello world", 5) < 0) {
    perror("server write");
    return;
  }

  if (close(sock) < 0) {
    perror("server close");
    return;
  }
}

void tcpClient() {
  Socket sock;
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (connect(sock.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) <
      0) {
    perror("client listen");
    return;
  }

  char msg[] = "goodbye";
  if (send(sock.get(), msg, sizeof(msg), 0) < 0) {
    perror("client send");
    return;
  }

  char buf[256] = {0};
  if (read(sock.get(), buf, sizeof(buf)) < 0) {
    perror("client read");
    return;
  }

  puts(buf);
}

int main() {
  std::thread th(tcpServer);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  tcpClient();
  th.join();
  return 0;
}
