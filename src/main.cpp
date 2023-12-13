#include <csignal>
#include <glib.h>
#include "Config.h"
#include "Zoom.h"

using namespace std;


/**
 *  Callback fired atexit()
 */
void onExit() {
    auto zoom = &Zoom::getInstance();
    zoom->leave();
    zoom->clean();

    cout << "exiting..." << endl;
}

/**
 * Callback fired when a signal is trapped
 * @param signal type of signal
 */
void onSignal(int signal) {
    onExit();
    _Exit(signal);
}


/**
 * Callback for glib event loop
 * @param data event data
 * @return always TRUE
 */
gboolean onTimeout (gpointer data) {
    return TRUE;
}

/**
 * Run the Zoom Meeting Bot
 * @param argc argument count
 * @param argv argument vector
 * @return SDKError
 */
SDKError run(int argc, char** argv) {
    SDKError err{SDKERR_SUCCESS};
    auto* zoom = &Zoom::getInstance();

    // read the CLI and config.ini file
    err = zoom->config(argc, argv);
    if (Zoom::hasError(err, "configure"))
        return err;

    // initialize the Zoom SDK
    err = zoom->init();
    if(Zoom::hasError(err, "initialize"))
        return err;

    // authorize with the Zoom SDK
    err = zoom->auth();
    if (Zoom::hasError(err, "authorize"))
        return err;

    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    atexit(onExit);

    return err;
}

int main(int argc, char **argv) {
    SDKError err = run(argc, argv);
    if (err != SDKERR_SUCCESS)
        return err;

    // Use an event loop to receive callbacks
    GMainLoop* eventLoop;
    eventLoop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(100, onTimeout, eventLoop);
    g_main_loop_run(eventLoop);

    return err;
}



