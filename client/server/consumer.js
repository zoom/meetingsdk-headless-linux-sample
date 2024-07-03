import * as net from 'node:net';
import { transcriber } from './transcribe.js';
import { appName } from '../config.js';

import debug from 'debug';
const dbg = debug(`${appName}:consumer`);

const socketPath = '/tmp/meeting.sock';
let socketClient = new net.Socket();
let interval = null;

const connect = () => socketClient.connect(socketPath);

const retry = () => {
    if (interval) return;
    dbg('Waiting for socket server...');
    interval = setInterval(connect, 10);
};

socketClient.on('connect', () => {
    dbg('connected to ' + socketPath);
    clearInterval(interval);
});

socketClient.on('error', (err) => {
    const { code } = err;

    if (['ECONNREFUSED', 'ENOENT'].includes(code)) retry();
    else dbg('connection error', code);
});

const run = async () => {
    try {
        connect();

        //dbg('Connecting to real-time transcript service');
        //await transcriber.connect();

        // Stop recording and close connection using Ctrl-C.
        process.on('SIGINT', async function () {
            //await transcriber.close();
            process.exit();
        });
    } catch (error) {
        console.error(error);
    }
};

export default {
    run,
    transcriber,
    socketClient
};
