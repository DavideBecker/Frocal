var socket = io()
var scaleDefault = 0
var scaleThreshold = 100
var lastScale = 0
var lastVisitors = 0
var activeArea
var INT_MAX = 32768
var resetTime = 1000 * 60 * 2 // ms - 2 minutes
var defaultPricePerKilo = 1.58 // EUR per KG

// Helper functions

function formatPrice(raw) {
    return Number(raw).toFixed(2).toLocaleString()
}

// Elements and Templates

function getId(id) {
    return document.getElementById(id)
}

function tpl(id) {
    return doT.template(getId('tpl-' + id).innerHTML)
}

var elems = {
    visitor: getId('visitor'),
    products: getId('products'),
    total: getId('total'),
    showDetails: getId('show-details'),

    tpl: {
        product: tpl('product')
    }
}

// Products

var products = []

class Product {
    constructor(name, pricePerKilo, weight) {
        this.name = name || ''
        this.weight = weight || 0
        this.pricePerKilo = pricePerKilo || defaultPricePerKilo
        products.push(this)
    }

    calcPrice() {
        return this.pricePerKilo / 1000 * this.weight
    }

    getData() {
        this.weight = Math.round(this.weight)

        return {
            name: this.name,
            weight: this.weight,
            price: this.calcPrice()
        }
    }

    getIndex() {
        return products.indexOf(this);
    }

    render(data) {
        var r = data || this.getData()
        return elems.tpl.product(r)
    }

    remove() {
        var i = this.getIndex()
        if (i !== -1) {
            products.splice(i, 1);
        }
    }
}

var dynamicProduct = new Product('Kartoffeln', 1.85)
new Product('Äpfel', 2.99, 53)
new Product('Tomaten', 4.59, 241)

// Areas

function switchArea(newArea) {
    activeArea = newArea
    document.body.dataset.section = newArea
}

// Data handling

function handleWeight(amount) {

    console.log({
        scaleDefault: scaleDefault,
        amount: amount,
        lastScale: lastScale,
        diff: lastScale - amount,
        scaleThreshold: scaleThreshold
    })

    if (Math.abs(lastScale - amount) > scaleThreshold) {
        resetDelay.reset()
        if (activeArea == 'welcome') {
            switchArea('main')
            scaleDefault = lastScale
        }
    }

    // console.log({
    //     full: activeArea == 'welcome' && Math.abs(scaleDefault - lastScale) > scaleThreshold,
    //     scaleDefault: scaleDefault,
    //     lastScale: lastScale,
    //     scaleThreshold: scaleThreshold,
    //     abs: Math.abs(scaleDefault - lastScale),
    //     bool: Math.abs(scaleDefault - lastScale) > scaleThreshold
    // })

    elems.products.innerHTML = ''

    dynamicProduct.weight = Math.round(Math.max(scaleDefault - amount, 0) / 3) * 3

    var total = 0

    for (var prod of products) {
        var productData = prod.getData()
        total += productData.price
        elems.products.innerHTML += prod.render(productData)
    }

    elems.total.innerHTML = formatPrice(total) + ' €'

    lastScale = amount
}

function handleVisitors(amount) {
    lastVisitors = +amount
    elems.visitor.innerHTML = +lastVisitors
    if (activeArea == 'intro') {
        resetDelay.start()
        switchArea('welcome')
    }
}

function handleData(response) {
    // console.log(response)
    if (response.sent == 1) {
        handleWeight(response.data)
    }

    if (response.sent == 2) {
        handleVisitors(response.data)
    }
}

// Generic timeout function

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
        if (this.timeout) {
            this.set()
        }
    }
}

var resetDelay = new Timeout(function () {
    scaleDefault = lastScale
    switchArea('intro')
}, resetTime)

socket.on('sensorData', function (data) {
    handleData(data)
})

switchArea('intro')

// Manual switching

document.addEventListener('keydown', function (e) {
    if (e.code == 'Digit1') {
        switchArea('intro')
    }

    if (e.code == 'Digit2') {
        switchArea('welcome')
    }

    if (e.code == 'Digit3') {
        switchArea('main')
    }

    if (e.code == 'Digit4') {
        switchArea('details')
    }

    if (e.code == 'Digit5') {
        handleData({
            sent: 2,
            data: lastVisitors + 1
        })
    }

    if (e.code == 'Digit6') {
        handleData({
            sent: 1,
            data: 200 + Math.random() * 2000
        })
    }

    if (e.code == 'Digit9') {
        resetDelay.trigger()
        resetDelay.stop()
    }
})

// elems.showDetails.addEventListener('click', function () {
//     switchArea('details')
// })



