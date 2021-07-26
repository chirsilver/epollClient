#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_EVENTS 100

class Epoll {
private:
    int epfd;
    int poll_num;
    epoll_event events[MAX_EVENTS];
public:
    Epoll();
    ~Epoll();
    int create(int size);
    int wait();
    int wait_timeout(int time);
    void over();
    int add_event(int fd, int event);
    int del_event(int fd, int event);
    int mod_event(int fd, int event);
    epoll_event* get_event(int idx);
};

#endif