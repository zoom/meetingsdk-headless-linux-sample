#include "SocketServer.h"

SocketServer::SocketServer() {
    pthread_mutex_init(&m_mutex, NULL);
}

SocketServer::~SocketServer() {
    cleanup();
}

void* SocketServer::threadCreate(void *obj) {
    return reinterpret_cast<SocketServer*>(obj)->run();
}

void SocketServer::threadKill(int sig, siginfo_t *info, void *ctx) {
    auto* self = &SocketServer::getInstance();

    self->stop();
}

void* SocketServer::run() {
    cout << "Starting socket server" << endl;
    if (pthread_mutex_trylock(&m_mutex) != 0) {
        cerr << "Unable to lock mutex" << endl;
        return nullptr;
    }

    /*
   * In case the program exited inadvertently on the last run,
   * remove the socket.
   */

    cleanup();


    /* Create local socket. */

    listenSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        cerr << "unable to create listen socket" << endl;
        return nullptr;
    }

    std::cout << "Listening on socket " << SOCKET_NAME << endl;

    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     */

    memset(&addr, 0, sizeof(struct sockaddr_un));

    /* Bind socket to socket name. */

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    auto ret = bind(listenSocket, (const struct sockaddr *) &addr,
                    sizeof(struct sockaddr_un));
    if (ret == -1) {
        cerr << "unable to bind socket" << endl;
        return nullptr;
    }

    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */

    ret = listen(listenSocket, 20);
    if (ret == -1) {
        cerr << "unable to listen on socket" << endl;
        return nullptr;
    }


    for (;;) {

        /* Wait for incoming connection. */

        dataSocket = accept(listenSocket, NULL, NULL);
        if (dataSocket == -1) {
            cerr << "failed to accept connection" << endl;
            return nullptr;
        }

        for(;;) {

            /* Wait for next data packet. */

            auto ret = read(dataSocket, buffer, 5);
            if (ret == -1) {
                cerr << "failed to read socket" << endl;
                return nullptr;
            }

            /* Ensure buffer is 0-terminated. */

            buffer[BUFFER_SIZE - 1] = 0;


            /* Handle commands. */
            ready = true;

        }
        return nullptr;
    }

    return nullptr;
}

bool SocketServer::isReady() {
    return ready;
}


int SocketServer::writeBuf(const char* buf, int len) {
    auto ret = write(dataSocket, buf, len);
    if (ret == -1) {
        cerr << "failed to write data" << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

int SocketServer::writeStr(const string& str) {
    auto buf = str.c_str();
    return writeBuf(buf, strlen(buf));
}

void SocketServer::cleanup () {
    if (access(SOCKET_NAME, F_OK) != -1) {
        unlink(SOCKET_NAME);
    }
}


int SocketServer::start() {

    cout << "creating thread..." << endl;
    if (pthread_create(&m_pid, NULL, &(SocketServer::threadCreate), this) != 0) {
        cerr << "unable to start thread" << endl;
        return false;
    }
    return true;
}

void SocketServer::stop() {
    cout << "Stopping" << endl;

    pthread_mutex_destroy(&m_mutex);
    if (m_pid) {
        pthread_cancel(m_pid);
        m_pid = 0;
    }
    if (listenSocket) {
        close(listenSocket);
        listenSocket = 0;
    }

    if (dataSocket) {
        close(dataSocket);
        dataSocket = 0;
    }

    cleanup();
}