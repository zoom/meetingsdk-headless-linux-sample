#define SOCKET_NAME "/tmp/test.sock"
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <iostream>

#include "Singleton.h"

using namespace std;


class SocketServer : public Singleton<SocketServer> {
    friend class Singleton<SocketServer>;

    struct sockaddr_un addr;

    int listenSocket;
    int dataSocket;
    char buffer[BUFFER_SIZE];

    pthread_t m_pid;
    pthread_mutex_t m_mutex;

    bool ready;

    void* run();
    static void* threadCreate(void* obj);
    static void threadKill(int sig, siginfo_t* info, void* ctx);

public:
    SocketServer();
    ~SocketServer();
    int start();
    void stop();

    int writeBuf(const char* buf, int len);
    int writeStr(const string& str);

    bool isReady();

    void cleanup();
};