const net = require('net');

const socket_path = '/tmp/test.sock';
let client = new net.Socket();


let interval = null


const connect = () => client.connect(socket_path)


const retry = () => {
    if (interval) return;
    console.log("Waiting for socket server...")
    interval = setInterval(connect,10)
}

client.on("connect", () => {
    console.log('connected to ' + socket_path);
    clearInterval(interval)

    client.write('READY');

});

client.on("error", (err) => {
    const { code } = err;

    if (["ECONNREFUSED", "ENOENT"].includes(code)) retry();
    else console.log("connection error", code)
})

client.on("data", function(data) {
    console.log('got data [%s]', data)
});

connect();

