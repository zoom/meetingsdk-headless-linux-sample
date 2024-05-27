'use strict';



const net = require('net');
const unixSocketServer = net.createServer();

unixSocketServer.listen('/tmp/mysocket', (s) => {
    console.log('now listening');
});

unixSocketServer.on('connection', (s) => {
    console.log('got connection!');
    s.write('hello world');
    s.end();
});