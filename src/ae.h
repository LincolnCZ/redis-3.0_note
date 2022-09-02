/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__

/*
 * 事件执行状态
 */
// 成功
#define AE_OK 0
// 出错
#define AE_ERR -1

/*
 * 文件事件状态
 */
// 未设置
#define AE_NONE 0
// 套接字可读  /*对应 epoll 中的 EPOLLIN*/
/*1.客户端对套接字执行write操作 2.客户端对套接字执行close操作
 * 3.客户端对服务器监听的套接字执行connect操作*/
#define AE_READABLE 1
// 套接字可写  /*对应 epoll 中的 EPOLLOUT EPOLLERR EPOLLHUP*/
/*1.客户端对套接字执行read操作（即，服务器端执行write操作）。
 * 注意：只有在有需要执行write操作的时候才设置，其他时候应该关闭*/
#define AE_WRITABLE 2

/*
 * 时间处理器的执行 flags
 */
// 文件事件
#define AE_FILE_EVENTS 1
// 时间事件
#define AE_TIME_EVENTS 2
// 所有事件
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
// 不阻塞，也不进行等待
#define AE_DONT_WAIT 4

/*
 * 决定时间事件是否要持续执行的 flag
 */
#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

/*
 * 事件处理器状态
 */
struct aeEventLoop;

/* Types and data structures 
 *
 * 事件接口
 */
typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask); /*文件事件处理函数*/
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData); /*时间事件处理函数*/
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);    /*时间事件释放函数*/
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop); /*在处理事件前要执行的函数*/

/* File event structure
 *
 * 文件事件结构
 */
typedef struct aeFileEvent {

    // 监听事件类型掩码，
    // 值可以是 AE_READABLE 或 AE_WRITABLE ，
    // 或者 AE_READABLE | AE_WRITABLE
    int mask; /* one of AE_(READABLE|WRITABLE) */

    // 读事件处理器
    aeFileProc *rfileProc;

    // 写事件处理器
    aeFileProc *wfileProc;

    // 多路复用库的私有数据
    void *clientData;

} aeFileEvent;

/* Time event structure
 *
 * 时间事件结构
 */
typedef struct aeTimeEvent {

    // 时间事件的唯一标识符
    long long id; /* time event identifier. */

    // 事件的到达时间
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */

    // 事件处理函数
    aeTimeProc *timeProc;

    // 事件释放函数
    aeEventFinalizerProc *finalizerProc;

    // 多路复用库的私有数据
    void *clientData;

    // 指向下个时间事件结构，形成链表
    struct aeTimeEvent *next;

} aeTimeEvent;

/* A fired event
 *
 * 已就绪事件
 */
typedef struct aeFiredEvent {

    // 已就绪文件描述符
    int fd;

    // 事件类型掩码，
    // 值可以是 AE_READABLE 或 AE_WRITABLE
    // 或者是两者的或
    int mask;

} aeFiredEvent;

/* State of an event based program 
 *
 * 事件处理器的状态
 */
typedef struct aeEventLoop {

    // 目前已注册的最大描述符
    int maxfd;   /* highest file descriptor currently registered */

    // 目前已追踪的最大描述符 /*其值 = server.maxclients（可以由用户指定）+REDIS_EVENTLOOP_FDSET_INCR*/
    int setsize; /* max number of file descriptors tracked */ /*events和fired数组的大小*/

    // 用于生成时间事件 id
    long long timeEventNextId;

    // 最后一次执行时间事件的时间
    time_t lastTime;     /* Used to detect system clock skew */

    // 已注册的文件事件
    aeFileEvent *events; /* Registered events */ /*数组，初始大小为setsize决定。数组的index为fd。*/

    // 已就绪的文件事件
    aeFiredEvent *fired; /* Fired events */ /*数组，初始大小为setsize决定。*/

    // 时间事件
    aeTimeEvent *timeEventHead; /*链表头*/

    // 事件处理器的开关
    int stop;

    // 多路复用库的私有数据
    void *apidata; /* This is used for polling API specific data */

    // 在处理事件前要执行的函数
    aeBeforeSleepProc *beforesleep;

} aeEventLoop;

/* Prototypes */
aeEventLoop *aeCreateEventLoop(int setsize); /*初始化事件处理器。setsize决定events和fired数组的大小*/
void aeDeleteEventLoop(aeEventLoop *eventLoop); /*删除事件处理器*/
void aeStop(aeEventLoop *eventLoop);

/**文件时间相关**/
/*函数接受一个套接字描述符、一个事件类型，以及一个事件处理器作为参数，将给定套接字的给定事件加入到I/O多路复用程序的监听范围之内，
 * 并对事件和事件处理器进行关联 。*/
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
                      aeFileProc *proc, void *clientData);
/*函数接受一个套接字描述符和一个监听事件类型作为参数，让 I/O 多路复用程序取消对给定套接字的给定事件的监听，并取消事件和事件处理器之间的关联。*/
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
/*函数接受一个套接字描述符，返回该套接宇正在被监听的事件类型:
 * 如果套接字没有任何事件被监听，那么函数返回 AE_NONE。
 * 如果套接字的读事件正在被监听，那么函数返回 AE_READABLE。
 * 如果套接字的写事件正在被监听，那么函数返回 AE_WRITABLE。
 * 如果套接字的读事件和写事件正在被监听，那么函数返回 AE_READABLE | AE_WRITABLE。
 * */
int aeGetFileEvents(aeEventLoop *eventLoop, int fd); /*获取给定 fd 正在监听的事件类型*/

/**时间事件相关**/
/*函数接受一个毫秒数 milliseconds 和一个时间事件处理器 proc 作为参数，将一个新的时间事件添加到服务器，这个新的时间事件将在当前时间的
 * milliseconds 毫秒之后到达，而事件的处理器为 proc。
 * 返回时间事件ID*/
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
/*函数接受一个时间事件 ID 作为参数，然后从服务器中删除该 ID 所对应的时间事件。*/
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);

/**主循环相关**/
/*文件事件分派器*/
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
/*函数接受一个套接字描述符、一个事件类型和一个毫秒数为参数，在给定的时间内阻塞并等待套接字的给定类型事件产生，
 * 当事件成功产生，或者等待超时之后，函数返回 。*/
int aeWait(int fd, int mask, long long milliseconds);
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
/*设置处理事件前需要被执行的函数*/
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);
int aeGetSetSize(aeEventLoop *eventLoop);
int aeResizeSetSize(aeEventLoop *eventLoop, int setsize);

#endif
