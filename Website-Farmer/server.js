var SerialPort = require("serialport");

var express = require('express');

var app = require('express')();
var server = require('http').Server(app);
var io = require('socket.io')(server);

const port = new SerialPort('/dev/cu.usbmodem1421', {
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