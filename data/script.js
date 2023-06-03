// Access the DOM elements
const valueDisplay = document.getElementById("sensor-value");

console.log("Connecting to WebSocket server");

// Create a WebSocket connection
const socket = new WebSocket("ws://" + window.location.hostname + ":81/");

// Handle received WebSocket messages
socket.onmessage = function (event) {
  console.log("Received message:", event.data);
  const sensorValue = event.data;
  valueDisplay.innerText = sensorValue;
};

socket.onopen = function (event) {
  console.log("WebSocket connection opened");
};

socket.onerror = function (event) {
  console.error("WebSocket error observed:", event);
};
