/*
 * Lthread
 * Copyright (C) 2012, Hasan Alayli <halayli@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * lthread_epoll.c
 */

#include "lthread_int.h"
#include <assert.h>
#include <string.h>

inline void
register_rd_interest(int fd)
{
    struct epoll_event ev;
    int ret = 0;
    sched_t *sched = lthread_get_sched();

    ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
    ev.data.fd = fd;
    ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_MOD, fd, &ev);
    if (ret < 0)
        ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_ADD, fd, &ev);
    assert(ret != -1);

    /* compute sched can have a NULL current_ltread */ 
    if (sched->current_lthread != NULL)
        lt->state |= BIT(LT_WAIT_READ);
}

inline void
register_wr_interest(int fd)
{
    struct epoll_event ev;
    int ret = 0;
    sched_t *sched = lthread_get_sched();

    ev.events = EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP;
    ev.data.fd = fd;
    ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_MOD, fd, &ev);
    if (ret < 0)
        ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_ADD, fd, &ev);
    assert(ret != -1);

    if (sched->lt != NULL)
        lt->state |= BIT(LT_WAIT_WRITE);
}

inline void
clear_wr_interest(int fd)
{
    struct epoll_event ev;
    int ret = 0;
    sched_t *sched = lthread_get_sched();

    ev.data.fd = fd;
    ev.events = EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP;
    ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_MOD, fd, &ev);
    assert(ret != -1);
}

inline void
clear_rd_interest(int fd)
{
    struct epoll_event ev;
    int ret = 0;
    sched_t *sched = lthread_get_sched();

    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
    ret = epoll_ctl(sched->poller_fd, EPOLL_CTL_MOD, fd, &ev);
    assert(ret != -1);
}

int
create_poller(void)
{
    return epoll_create(1024);
}

inline int
poll_events(struct timespec t)
{
    sched_t *sched = lthread_get_sched();

    return epoll_wait(sched->poller_fd, sched->eventlist, LT_MAX_EVENTS,
        t.tv_sec*1000 + t.tv_nsec/1000000);
}

inline int
get_fd(struct epoll_event *ev)
{
    return ev->data.fd;
}

inline int
get_event(struct epoll_event *ev)
{
    return ev->events;
}

inline int
is_eof(struct epoll_event *ev)
{
    return ev->events & EPOLLHUP;
}

inline int
is_write(struct epoll_event *ev)
{
    return ev->events & EPOLLOUT;
}

inline int
is_read(struct epoll_event *ev)
{
    return ev->events & EPOLLIN;
}
