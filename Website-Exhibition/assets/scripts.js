var socket = io()
var scaleDefault = 0
var scaleThreshold = 100
var lastScale = 0
var lastVisitors = 0
var activeArea
var INT_MAX = 32768
var resetTime = 5000 // 5 minutes

function switchArea(newArea) {
    activeArea = newArea
    document.body.dataset.section = newArea
}

function handleWeight(amount) {
    lastScale = amount

    console.log({
        full: activeArea == 'welcome' && Math.abs(scaleDefault - lastScale) > scaleThreshold,
        scaleDefault: scaleDefault,
        lastScale: lastScale,
        scaleThreshold: scaleThreshold,
        abs: Math.abs(scaleDefault - lastScale),
        bool: Math.abs(scaleDefault - lastScale) > scaleThreshold
    })

    resetDelay.reset()

    if(activeArea == 'welcome' && Math.abs(scaleDefault - lastScale) > scaleThreshold) {
        switchArea('main')
    }
}

function handleVisitors(amount) {
    lastVisitors = amount
    if(activeArea == 'intro') {
        resetDelay.start()
        switchArea('welcome')
    }
}

function handleData(response) {
    if(response.sent == 1) {
        handleWeight(response.data)
    }

    if(response.sent == 2) {
        handleVisitors(response.data)
    }
}

class Timeout {
    constructor(func, delay) {
        this.func = func
        this.delay = delay
        this.timeout = false
        return this
    }

    start() {
        this.set()
    }

    stop() {
        this.timeout = false
    }

    trigger() {
        this.func()
    }

    set() {
        window.clearTimeout(this.timeout)
        this.timeout = window.setTimeout(this.func, this.delay)
    }

    reset() {
        if(this.timeout) {
            this.set()
        }
    }
}

var resetDelay = new Timeout(function() {
    scaleDefault = lastScale
    switchArea('intro')
}, resetTime)

// socket.on('sensorData', function(data) {
//     handleData(data)
// })

switchArea('intro')


document.addEventListener('keydown', function(e) {
    console.log(e)
    if(e.code == 'Digit1') {
        switchArea('intro')
    }

    if(e.code == 'Digit2') {
        switchArea('welcome')
    }

    if(e.code == 'Digit3') {
        switchArea('main')
    }

    if(e.code == 'Digit4') {
        switchArea('details')
    }

    if(e.code == 'Digit5') {
        handleData({
            sent: 1,
            data: 400
        })
    }

    if(e.code == 'Digit6') {
        handleData({
            sent: 1,
            data: 600
        })
    }

    if(e.code == 'Digit7') {
        handleData({
            sent: 2,
            data: 10
        })
    }
})