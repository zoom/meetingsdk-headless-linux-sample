#include <csignal>
#include <glib.h>
#include "Config.h"
#include "Zoom.h"

using namespace std;

// Function pointer for the signal trap callback
function<void(int)> signalFunc;

/**
 * Callback used when a signal is trapped
 * @param signal
 */
void onSignal(int signal) {
    signalFunc(signal);
}

/**
 * Used as a callback when the glib main loop times out
 * @param data callback data
 * @return gboolean TRUE
 */
gboolean onTimeout (gpointer data) {
    return TRUE;
}

/**
 * Configure Zoom using command line arguments
 * @param argc argument count
 * @param argv argument vector
 * @return SDKERR_SUCCESS on success
 */
SDKError configure(int argc, char** argv) {
    Config conf;

    int status = conf.read(argc, argv);
    if (status) {
        cerr << "failed to read configuration" << endl;
        return SDKERR_INTERNAL_ERROR;
    }

    SDKError err{SDKERR_SUCCESS};
    Zoom zoom(&conf);

    err = zoom.init();
    if(Zoom::hasError(err, "init()"))
        return err;

    function<void()> onAuth = [&zoom]() {
        SDKError err = zoom.startOrJoin();
        Zoom::hasError(err, "startOrJoin()");
    };

    err = zoom.auth(onAuth);
    if (Zoom::hasError(err, "auth()"))
        return err;

    signalFunc = [&zoom](int signal) {
        zoom.leave();
        zoom.clean();
        _Exit(signal);
    };

    signal(SIGINT, onSignal);
    signal(SIGABRT, onSignal);

    return err;
}

int main(int argc, char **argv) {
    SDKError err = configure(argc, argv);

    if (err != SDKERR_SUCCESS)
        return err;

    GMainLoop* eventLoop;
    eventLoop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(100, onTimeout, eventLoop);
    g_main_loop_run(eventLoop);

    return err;
}



