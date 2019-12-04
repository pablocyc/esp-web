let connection = new WebSocket('ws://' + location.hostname + ':81/', [
  'arduino'
])
connection.onopen = function() {
  connection.send('Connect ' + new Date())
}
connection.onerror = function(error) {
  console.log('WebSocket Error ', error)
}
connection.onmessage = function(e) {
  console.log('Server: ', e.data)
}
function sendRGB() {
  let r = parseInt(document.getElementById('r').value).toString(16)
  let g = parseInt(document.getElementById('g').value).toString(16)
  let b = parseInt(document.getElementById('b').value).toString(16)
  if (r.length < 2) {
    r = '0' + r
  }
  if (g.length < 2) {
    g = '0' + g
  }
  if (b.length < 2) {
    b = '0' + b
  }
  let rgb = '#' + r + g + b
  console.log('RGB: ' + rgb)
  connection.send(rgb)
}
