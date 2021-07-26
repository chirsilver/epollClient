#include "epoll.h"
#include "tlvpacket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

#define STOP 0
#define START 1
#define KEEPALIVE 2
#define BUFFER_SIZE 1024

#define ASSERT(exep) ((exep)?({void(0);}):({return 1;}))
#define IPADDR "127.0.0.1"
#define PORT 8888

int Socket();
int Connect(const char *ip, const int port);
void Run();
void Stdin();
void Read(int fd);
void Write(int fd, TLVPacket *packet);
int reprase(const char *str, int len);
void Over();

int listenfd = -1, status = STOP;
Epoll ep;

int main(int argc, char **argv) {
    int ret = Socket();
    ASSERT(!ret);
    puts("socket create success...");

    ret = Connect(IPADDR, PORT);
    ASSERT(!ret);
    printf("connect to server[%s:%d] success...\n", IPADDR, PORT);

    ret = ep.create(5);
    ASSERT(!ret);
    puts("epoll create success...");

    ep.add_event(0, EPOLLIN);
    ep.add_event(listenfd, EPOLLIN);

    Run();
    Over();
    
    return 0;
}

void Run() {
    status = START;
    while(status != STOP) {
        int ret = ep.wait();
        for(int i = 0; i < ret; i++) {
            epoll_event *event = ep.get_event(i);
            if(event->data.fd == 0) {
                Stdin();
            } else if(event->data.fd == listenfd && event->events & EPOLLIN) {
                Read(event->data.fd);
            }
        }
    }
}

void Stdin() {
    char buf[BUFFER_SIZE];
    int len = read(0, buf, BUFFER_SIZE);
    if(len <= 0) return;
    buf[len-1] = '\0';  // erase \n
    int space = -1;
    for(int i = 0; i < len-1; i++) {
        if(buf[i] == ' ') {
            buf[i] = '\0';
            space = i; 
            break;
        }
        if('a' <= buf[i] && buf[i] <= 'z') buf[i] -= 32;
    }
    if(space == -1) {
        if(buf[0] == 'Q' || buf[0] == 'q') { Over(); return; }
        printf("\033[01;31mError\033[0m: the format is wrong, the correct is: [DATA | ERROR] msg.\n");
        return;
    }

    TLVPacket packet;
    packet.node.type = reprase(buf);
    packet.node.length = strlen(buf+space+1);
    strcpy(packet.node.value, buf+space+1);

    printf("send to server a msg{type:%s;length:%d}: %s.\n", prase(packet.node.type), packet.node.length, packet.node.value);

    Write(listenfd, &packet);
}

void Read(int fd) {
    char buf[BUFFER_SIZE];
    int len = read(fd, buf, BUFFER_SIZE-1);
    if(len < 0) return;
    if(len == 0) {
        puts("server close.");
        Over();
        return;
    }

    buf[len+1] = '\0';
    TLVPacket packet;
    bzero(packet.data, sizeof(packet.data));
    strncpy(packet.data, buf, BUFFER_SIZE);

    printf("recv from sever a msg{type:%s;length:%d}: %s\n", prase(packet.node.type), packet.node.length, packet.node.value);

}

void Write(int fd, TLVPacket *packet) {
    write(fd, packet->data, sizeof(packet->data));
}

int Connect(const char *ip, const int port) {
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int ret = inet_aton(ip, &addr.sin_addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    ret = connect(listenfd, (sockaddr*)&addr, sizeof(addr));
    ASSERT(ret != -1);

    return 0;
}

int Socket() {
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    ASSERT(listenfd >= 0);

    int reuse = 1;
    int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ASSERT(ret != -1);

    return 0;
}

void Over() {
    if(status == STOP) return;
    status = STOP;
    close(listenfd);
    listenfd = -1;
}