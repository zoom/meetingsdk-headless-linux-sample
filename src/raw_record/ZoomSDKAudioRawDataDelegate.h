
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H

#include <iostream>
#include <fstream>
#include <sstream>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_audio_helper_interface.h"

 #include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;


    websocketpp::lib::error_code ec;

    c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}

void transcribeThread(AudioRawData* data) {
        // Create a client endpoint
    client c;

    std::string uri = "ws://localhost:9002";

    if (argc == 2) {
        uri = argv[1];
    }

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(&on_message,&c,::_1,::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}


class ZoomSDKAudioRawDataDelegate : public IZoomSDKAudioRawDataDelegate {
    string m_dir = "out";
    string m_filename = "test.pcm";
    bool m_useMixedAudio;

    void writeToFile(const string& path, AudioRawData* data);
public:
    ZoomSDKAudioRawDataDelegate(bool useMixedAudio);

    void setDir(const string& dir);
    void setFilename(const string& filename);

    void onMixedAudioRawDataReceived(AudioRawData* data) override;
    void onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) override;
    void onShareAudioRawDataReceived(AudioRawData* data) override;
};



#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H
