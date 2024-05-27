const fs = require('fs');
const net = require('net');

const socket_path = '/tmp/test.sock';
let client = new net.Socket();

const writableStream = fs.createWriteStream("out/test.raw");
//writableStream.on('error',  (error) => {
//    console.log(`An error occured while writing to the file. Error: ${error.message}`);
//});

let interval = null

const connect = () =>
    client.connect(socket_path)


const retry = () => {
    if (interval) return;
    console.log("Waiting for socket server...")
    interval = setInterval(connect,10)
}

client.on("connect", () => {
    console.log('connected to ' + socket_path);
    clearInterval(interval)
});

client.on("error", (err) => {
    const { code } = err;

    if (["ECONNREFUSED", "ENOENT"].includes(code)) retry();
    else console.log("connection error", code)
})

client.on("data", (data) => {
    console.log("received data", data);
    fs.appendFile("out/test.raw", data, function(err) {
        if(err) {
            return console.log(err);
        }
        console.log("The file was saved!");
    });
});

client.on("disconnect", () => {
    console.log("disconnected", socket_path)
    writableStream.end();

})

connect();