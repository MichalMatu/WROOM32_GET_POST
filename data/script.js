// Access the DOM elements
const valueDisplay = document.getElementById("sensor-value");

// Create a WebSocket connection
const socket = new WebSocket("ws://" + window.location.hostname + ":81/");

// const socket = new WebSocket("ws://" + window.location.host + "/");

// Handle received WebSocket messages
socket.onmessage = function (event) {
  const sensorValue = event.data;
  valueDisplay.innerText = sensorValue;
};
