var SerialPort = require("serialport");

var express = require('express');

var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);

// var serialportName = '/dev/cu.usbmodem1411'
var serialportName = 'COM6'

const port = new SerialPort(serialportName, {
    // baudRate: 38400
    // baudRate: 115200
    baudRate: 9600
});
if(port) {
    const parser = new SerialPort.parsers.Readline();
    port.pipe(parser);
    parser.on('data', function(data) {
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
}

io.on('connection', (socket) => {
    console.log('user connected')
});

app.use('/assets', express.static(__dirname + '/assets'));

app.get('/', function(req, res,next) {
    res.sendFile(__dirname + '/index.html');
});

server.listen(8080);
