//
// Created by ht on 17-7-31.
//

#include "ProcessPool.h"

void detail::setNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flag);
    assert(ret == 0);
}

void detail::addFd(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    detail::setNonBlocking(fd);
}

void detail::deleteFd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void detail::sigHandler(void *sig)
{
    int save_errno = errno;
    int *ptr = sig;
    int msg = static_cast<int>(*ptr);
    send(detail::sigPipeFd[1], (void *) &msg, 1, 0);
    errno = save_errno;
}

void detail::addSignal(int sig, void(*handler)(void *), bool restart)
{
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    //sa.sa_handler = handler;
    if (restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}







