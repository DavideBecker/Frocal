var SerialPort = require("serialport");
const Readline = require('@serialport/parser-readline')

var express = require('express');

var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);

var serialportName = process.argv || '/dev/cu.usbmodem1411'
// var serialportName = 'COM6'
function reconnectArduino() {
    console.log('INITIATING RECONNECT');
    setTimeout(function () {
        console.log('RECONNECTING TO ARDUINO');
        connectToArduino();
    }, 2000);
}

function connectToArduino() {
    const port = new SerialPort(serialportName, {
        // baudRate: 38400
        // baudRate: 115200
        baudRate: 9600
    });
    const parser = port.pipe(new Readline({ delimiter: '\r\n' }))

    if (parser) {
        parser.on('data', function (data) {
            var parsedData
            try {
                parsedData = JSON.parse(data.toString())
                console.log(parsedData)
            } catch (e) {
                console.error('!', data);
                return false;
            }

            return io.emit('sensorData', parsedData)
        });

        port.on('close', function () {
            console.log('ARDUINO PORT CLOSED');
            reconnectArduino();
        });

        port.on('error', function (err) {
            console.error("! ERROR", err);
            reconnectArduino();
        });
    }
}

connectToArduino()

io.on('connection', (socket) => {
    console.log('user connected')
});

app.use('/assets', express.static(__dirname + '/assets'));

app.get('/', function(req, res, next) {
    res.sendFile(__dirname + '/index.html');
});

server.listen(8080);