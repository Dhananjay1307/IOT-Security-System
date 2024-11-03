#include <WiFi.h>
#include <PubSubClient.h>


//#include <ESP32Servo.h>


// Update these with your Wi-Fi credentials and MQTT broker address
const char* ssid = "Amshu GT 2";
const char* password = "Amshu@399";
const char* mqtt_server = "192.168.171.86";  // Use your laptop's IP address

WiFiClient espClient;
PubSubClient client(espClient);

// Motor A pins
#define IN1 16
#define IN2 17
#define ENA 21  // Motor A enable pin

// Motor B pins
#define IN3 18
#define IN4 19
#define ENB 22  // Motor B enable pin

// Servo motor pin
//#define servoPin 23

// Servo object
//Servo myservo;

// Define distance per matrix cell in cm
#define CELL_DISTANCE 10


void moveForward(int distance) {
  int time = calculateTime(distance);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(time);
  stopMotors();
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(500); // Adjust this delay for a proper 90-degree turn
  stopMotors();
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(500); // Adjust this delay for a proper 90-degree turn
  stopMotors();
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Callback function when a message is received
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);

  // Handle different control messages
  if (message == "left") {
    // Code to turn left
    turnLeft();
  }
  else if (message == "right") {
    // Code to turn right
    turnRight();
  }
  else if (message == "forward") {
    // Code to go forward
    moveForward(CELL_DISTANCE);
  }
}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect to the MQTT broker
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe to the control topic
      client.subscribe("esp32/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // Motor pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Attach the servo motor to its pin
  //myservo.attach(servoPin);

  // Set motor speed to full
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Center the servo motor (looking straight ahead)
  //myservo.write(90);
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // Connect to the broker running on your laptop
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Calculate time to move forward based on distance
int calculateTime(int distance) {
  int time = (distance / 25) * 1000; // Assume 1 second for 10 cm
  return time;
}

