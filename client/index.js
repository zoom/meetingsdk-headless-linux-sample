
import * as net from "net";

import { Readable } from 'stream'
import { AssemblyAI } from 'assemblyai'

const socket_path = '/tmp/test.sock';
let socketClient = new net.Socket();
let interval = null

const connect = () =>
    socketClient.connect(socket_path)

const retry = () => {
    if (interval) return;
    console.log("Waiting for socket server...")
    interval = setInterval(connect,10)
}

socketClient.on("connect", () => {
    console.log('connected to ' + socket_path);
    clearInterval(interval)
});

socketClient.on("error", (err) => {
    const { code } = err;

    if (["ECONNREFUSED", "ENOENT"].includes(code)) retry();
    else console.log("connection error", code)
})

socketClient.on("disconnect", () => {
    console.log("disconnected", socket_path)
})

const client = new AssemblyAI({
    apiKey: '***REMOVED***',
    wordBoost:['zoom', 'meeting', 'sdk', 'transcription']
})

const transcriber = client.realtime.transcriber({
    sampleRate: 32_000
})

transcriber.on('open', ({ sessionId }) => {
    console.log(`Session opened with ID: ${sessionId}`)
})

transcriber.on('error', (error) => {
    console.error('Error:', error)
})

transcriber.on('close', (code, reason) =>
    console.log('Session closed:', code, reason)
)

transcriber.on('transcript', (transcript) => {
    if (!transcript.text) {
        return
    }

    if (transcript.message_type === 'PartialTranscript') {
        console.log('Partial:', transcript.text)
    } else {
        console.log('Final:', transcript.text)
    }
})

const run = async () => {

    try {
        connect();

        console.log('Connecting to real-time transcript service')
        await transcriber.connect()

        socketClient.on("data", (data) => {
           transcriber.sendAudio(data);
        });

        // Stop recording and close connection using Ctrl-C.
        process.on('SIGINT', async function () {
            await transcriber.close()
            process.exit()
        })
    } catch (error) {
        console.error(error)
    }
}


run().catch(err => console.error(err))
