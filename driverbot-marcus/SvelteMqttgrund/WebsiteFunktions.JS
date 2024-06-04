// Function to remove activeKey class from all keys after transition
function removeActiveClass(e) {
  e.target.classList.remove("activeKey");
}

let clientID;
let client;
let isConnected = false; // Flag to track connection status
let topic = "marcus.persson@hitachigymnasiet.se/driverbot";

// Generate a random client ID
clientID = "clientId-QNXfdTpOzc88" + parseInt(Math.random() * 100);

// Define host and port
let host = "maqiatto.com";
let port = 8883;

client = new Paho.MQTT.Client(host, Number(port), clientID);

function startMqttConnection(clientID, client, isConnected, topic) {
  client.onConnectionLost = onConnectionLost;
  client.onMessageArrived = onMessageArrived;
  console.log("connecting");
  client.connect({
      userName: "marcus.persson@hitachigymnasiet.se",
      password: "GreekMythology",
      onSuccess: onMqttConnect,
      onFailure: onMqttConnectFail
  });
  console.log(client);
}

// Array of messages for random text display
const textArray = ["Try Again"];

// Function to generate a random number within the textArray length
function getRandomNumber() {
  return Math.floor(Math.random() * textArray.length);
}

// Function to get a random text from textArray
function getRandomText() {
  const index = getRandomNumber();
  return textArray[index];
}

// Function to update the message element based on the key pressed
function updateMessageText(keyCode) {
  const messageElement = document.getElementById("message");
  if ([87, 65, 83, 68, 81, 69, 82].includes(keyCode)) {
      messageElement.innerHTML = "Key pressed: " + String.fromCharCode(keyCode);
  } else {
      messageElement.innerHTML = getRandomText();
  }
}

// Function to add activeKey class to the key pressed
function activateKey(e) {
  const key = document.querySelector(`.key[data-key="${e.keyCode}"]`);
  if (key) {
      key.classList.add("activeKey");
  }
}

// Variables for recording state and key sequence with timestamps
let isRecording = false;
let recordedKeys = [];

// Function to handle key press events
function handleKeyPress(event) {
  const keyCode = event.keyCode;

  // Toggle recording on 'R' key press
  if (keyCode === 82) {
      isRecording = !isRecording;
      const indicator = document.getElementById("recording-indicator");
      if (isRecording) {
          indicator.classList.add("recording");
          console.log("Recording started.");
      } else {
          indicator.classList.remove("recording");
          sendRecordedKeys(); // Send recorded keys in the correct order
          recordedKeys = [];
      }
  } else {
      // Handle key press actions
      if (!isRecording) {
          sendKeyToBroker(event.key);
      } else {
          const timestamp = Date.now();
          recordedKeys.push({ key: event.key, timestamp: timestamp });
      }

      updateMessageText(keyCode);
  }

  activateKey(event); // Highlight the pressed key
}

// Function to send a key press to the MQTT broker
function sendKeyToBroker(key) {
  console.log("Sending Key to Broker:", key);
  if (client.isConnected()) {
      if (['w', 'a', 's', 'd', 'q', 'e'].includes(key.toLowerCase())) {
          client.send(topic, key);
      }
  } else {
      console.log("Client not connected. Attempting to reconnect...");
      client.connect({
          onSuccess: function() {
              console.log("Reconnected to MQTT broker");
              client.send(topic, key);
          },
          onFailure: function(error) {
              console.error("Reconnection failed: ", error);
          }
      });
  }
}

// Function to send recorded keys with their respective delays
function sendRecordedKeys() {
  if (recordedKeys.length === 0) return;

  let previousTimestamp = recordedKeys[0].timestamp;

  recordedKeys.forEach((entry, index) => {
      const delay = entry.timestamp - previousTimestamp;
      previousTimestamp = entry.timestamp;

      setTimeout(() => {
          sendKeyToBroker(entry.key);
      }, delay * index); // Scale delay with index to ensure correct order
  });
}

// Add event listener for key press events
window.addEventListener("keydown", handleKeyPress);

console.log("starting connection");
startMqttConnection(clientID, client, isConnected, topic);
console.log("has run startMqttConnection");

// Callback function for successful connection
function onMqttConnect() {
  isConnected = true;
  console.log("Connected to MQTT broker");
}

// Callback function for when the connection is lost
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
      console.log("Connection lost:", responseObject.errorMessage);
  }
}

// Callback function for when a message arrives
function onMessageArrived(message) {
  console.log("Message arrived:", message.payloadString);
}

// Callback function for connection failure
function onMqttConnectFail() {
  document.getElementById("messages").innerHTML += '<span>ERROR: Connection to: ' + host + ' on port: ' + port + ' failed.</span><br/>';
}

// Listens for the transition end event and removes the activeKey class
const keys = Array.from(document.querySelectorAll(".key"));
keys.forEach(key => key.addEventListener("transitionend", removeActiveClass));
