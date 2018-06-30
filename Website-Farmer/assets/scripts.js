var socket = io()

var visits = document.getElementById('visits')

socket.on('sensorData', function(data) {
    console.log(data);
    if(data.type == 1) {
        var r = data.value / 20000 * 150
        document.body.style.backgroundColor = 'hsl('+r+', 100%, 50%)';
    }

    if(data.type == 2) {
        visits.innerHTML = data.value + ' Besucher'
    }
})