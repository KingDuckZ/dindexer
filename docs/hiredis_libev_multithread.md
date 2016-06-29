[TOC]

# Using hiredis+libev in a separate thread for events

Used sources:

1. [stackoverflow.com/questions/14621261][link_so_1]
2. [stackoverflow.com/questions/8611126][link_so_2]
3. [libev thread locking example][link_ev_doc]


## Explanation

### Link 1

Possibly due to the original question asked, [Link 1][link_so_1] seems to show that you need to have at least two ev_loop objects created in order to run one in a separate thread. The code on that page:

```cpp
//This program is demo for using pthreads with libev.
//Try using Timeout values as large as 1.0 and as small as 0.000001
//and notice the difference in the output

//(c) 2009 debuguo
//(c) 2013 enthusiasticgeek for stack overflow
//Free to distribute and improve the code. Leave credits intact

#include <ev.h>
#include <stdio.h> // for puts
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;
double timeout = 0.00001;
ev_timer timeout_watcher;
int timeout_count = 0;

ev_async async_watcher;
int async_count = 0;

struct ev_loop* loop2;

void* loop2thread(void* args)
{
    printf("Inside loop 2"); // Here one could initiate another timeout watcher
    ev_loop(loop2, 0);       // similar to the main loop - call it say timeout_cb1
    return NULL;
}

static void async_cb (EV_P_ ev_async *w, int revents)
{
    //puts ("async ready");
    pthread_mutex_lock(&lock);     //Don't forget locking
    ++async_count;
    printf("async = %d, timeout = %d \n", async_count, timeout_count);
    pthread_mutex_unlock(&lock);   //Don't forget unlocking
}

static void timeout_cb (EV_P_ ev_timer *w, int revents) // Timer callback function
{
    //puts ("timeout");
    if (ev_async_pending(&async_watcher)==false) { //the event has not yet been processed (or even noted) by the event loop? (i.e. Is it serviced? If yes then proceed to)
        ev_async_send(loop2, &async_watcher); //Sends/signals/activates the given ev_async watcher, that is, feeds an EV_ASYNC event on the watcher into the event loop.
    }

    pthread_mutex_lock(&lock);     //Don't forget locking
    ++timeout_count;
    pthread_mutex_unlock(&lock);   //Don't forget unlocking
    w->repeat = timeout;
    ev_timer_again(loop, &timeout_watcher); //Start the timer again.
}

int main (int argc, char** argv)
{
    if (argc < 2) {
        puts("Timeout value missing.\n./demo <timeout>");
        return -1;
    }
    timeout = atof(argv[1]);

    struct ev_loop *loop = EV_DEFAULT;  //or ev_default_loop (0);

    //Initialize pthread
    pthread_mutex_init(&lock, NULL);
    pthread_t thread;

    // This loop sits in the pthread
    loop2 = ev_loop_new(0);

    //This block is specifically used pre-empting thread (i.e. temporary interruption and suspension of a task, without asking for its cooperation, with the intention to resume that task later.)
    //This takes into account thread safety
    ev_async_init(&async_watcher, async_cb);
    ev_async_start(loop2, &async_watcher);
    pthread_create(&thread, NULL, loop2thread, NULL);

    ev_timer_init (&timeout_watcher, timeout_cb, timeout, 0.); // Non repeating timer. The timer starts repeating in the timeout callback function
    ev_timer_start (loop, &timeout_watcher);

    // now wait for events to arrive
    ev_loop(loop, 0);
    //Wait on threads for execution
    pthread_join(thread, NULL);

    pthread_mutex_destroy(&lock);
    return 0;
}
```

with the comment *"Note for libev 4+ ev_loop should be ev_run."* is still slightly useful but it shouldn't be taken as a model.


### Link 2

The [second link][link_so_2] is what shows that one ev_loop is enough. This is the code provided by the original poster:

```cpp
    void RedisSubscriber::Start() {
        m_redis = redisAsyncConnect(m_addr.c_str(),m_port);
        m_redis->data = (void*)this;

        m_loop = ev_loop_new(EVFLAG_NOINOTIFY);
        redisLibevAttach(m_loop, m_redis);
        redisAsyncSetConnectCallback(m_redis,connectCallback);
        redisAsyncSetDisconnectCallback(m_redis,disconnectCallback);
        redisAsyncCommand(m_redis, subscribeCallback, NULL, "SUBSCRIBE %s", m_channel.c_str());

        m_thread = boost::thread(ev_loop,m_loop,0);
    }

    void RedisSubscriber::Stop() {
        redisAsyncFree(m_redis);
        m_thread.join();
        m_redis = 0;
    }

    void RedisSubscriber::connectCallback(const redisAsyncContext *c) {

    }

    void RedisSubscriber::disconnectCallback(const redisAsyncContext *c, int status) {
        RedisSubscriber* r = (RedisSubscriber*)(c->data);
        ev_unloop(r->m_loop,EVUNLOOP_ALL);
    }

    void RedisSubscriber::subscribeCallback(redisAsyncContext *c, void *r, void *privdata) {

    }
```

There are no accepted answers, but the answer from *themoondothshine* provides very useful info. Here is what it says:

Assuming that you mean ev_run for your boost::thread, here's what you can do:

1. Setup an `ev_async`
2. In the callback of `ev_async` call `ev_break`.
3. Call `ev_async_send` from `RedisSubscriber::Stop()`. `ev_async` watchers are thread-safe -- it uses memory barriers for synchronising between threads.

This will cause the event loop to stop, and `m_thread.join()` will return.


### Link 3

The [THREAD LOCKING EXAMPLE][link_ev_doc] shows how to lock in order to protect the ev_loop object in use.

[link_so_1]: http://stackoverflow.com/questions/14621261/using-libev-with-multiple-threads#14779930
[link_so_2]: http://stackoverflow.com/questions/8611126/hiredis-libev-and-boostthreads
[link_ev_doc]: http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod#THREAD_LOCKING_EXAMPLE
