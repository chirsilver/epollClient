#include "epoll.h"

Epoll::Epoll() {}

Epoll::~Epoll() {
    over();
}

int Epoll::create(int size) {
    epfd = epoll_create(size);
    if(epfd <= 0) return -1;
    return 0;
}

void Epoll::over() {
    if(epfd <= 0) return;
    close(epfd);
    epfd = -1;
}

int Epoll::wait() {
    poll_num = epoll_wait(epfd, events, MAX_EVENTS, -1);
    return poll_num;
}

int Epoll::wait_timeout(int time) {
    poll_num = epoll_wait(epfd, events, MAX_EVENTS, time);
    return poll_num;
}

int Epoll::add_event(int fd, int event) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = event;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int Epoll::del_event(int fd, int event) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = event;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
}

int Epoll::mod_event(int fd, int event) {
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = event;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}

epoll_event* Epoll::get_event(int idx) {
    return &events[idx];
}