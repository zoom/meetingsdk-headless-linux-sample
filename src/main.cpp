#include <csignal>
#include <glib.h>
#include "Config.h"
#include "Zoom.h"

using namespace std;

/**
 * Callback fired when a signal is trapped
 * @param signal
 */
void onSignal(int signal) {
    auto zoom = &Zoom::getInstance();
    zoom->leave();
    zoom->clean();

    cout << "exiting..." << endl;
    _Exit(signal);
}

/**
 * Callback fired when Zoom SDK authorization completes
 */
void onAuth() {
    auto zoom = &Zoom::getInstance();
    auto err = zoom->startOrJoin();

    if (Zoom::hasError(err)) abort();
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
 * @return SDKError
 */
SDKError configure(int argc, char** argv) {
    SDKError err{SDKERR_SUCCESS};
    auto* zoom = &Zoom::getInstance();

    err = zoom->config(argc, argv);
    if (Zoom::hasError(err, "configure"))
        return err;

    err = zoom->init();
    if(Zoom::hasError(err, "initialize"))
        return err;

    err = zoom->auth(onAuth);
    if (Zoom::hasError(err, "authorize"))
        return err;

    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);
    signal(SIGKILL, onSignal);
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



