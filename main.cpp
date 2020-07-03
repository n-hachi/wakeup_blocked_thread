#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class Worker {
private:
  int sock_;

public:
  Worker(const char *ip, const int port);
};

Worker::Worker(const char *ip, const int port) {
  sock_ = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  bind(sock_, (struct sockaddr *)&addr, sizeof(addr));
}

int main(int argc, char const *argv[]) {
  Worker worker("0.0.0.0", 10000);
  return 0;
}
