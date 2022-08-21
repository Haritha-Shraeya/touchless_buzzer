#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// GPIO 12 D6
const int SENSOR = 12;

// GPIO 13 D7
//const int BUZZER = 13;

//WiFi
const char *ssid = "Shraeya_Wifi";
const char *password = "Merlion@2009";

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "sensor";
const char *mqtt_username = "emqx";
const char *mqtt_password = "buzzer";
const int mqtt_port = 1883;

// Client for MQTT set-up
WiFiClient espClient;
PubSubClient client(espClient);

// time intervals
const int   RECHECK_INTERVAL      = 5000;
const int   PUBLISH_INTERVAL      = 5000;
long        lastReconnectAttempt  = 0;
long        lastPublishAttempt    = 0;


// put your setup code here, to run once:
void setup() {

  //starts serial communication - esp8266 can communicate through USB connection. 115200 signifies how fast data is sent
  Serial.begin(115200);
  
  //configure pins to behave as input/output
  pinMode(SENSOR, INPUT);
//  pinMode(BUZZER, OUTPUT);

  //connecting to WiFi network
  WiFi.mode(WIFI_STA); //setting esp8266 to be a station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print("WiFi status:");
    Serial.println(WiFi.status());
    delay(5000);
  }
  Serial.println("Connected to WiFi network");

  //connecting to MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  
  //callback function handles incoming message from subscribed topic
  client.setCallback(callback);

  while (!client.connected()){
    if (client.connect("esp8266", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    }
    else {
      Serial.print("Failed at start:");
      Serial.println(client.state());
      delay(2000); // 2 seconds = 2000 milliseconds
    }

    //subscribing to topic sensor to receive data from sensor
    client.subscribe(topic);
  }
}

//callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

// put your main code here, to run repeatedly:
void loop() {
  long now = millis();
  
  if(client.connected()) 
  {
    if(now - lastPublishAttempt > PUBLISH_INTERVAL)
    {
      lastPublishAttempt = now;
      char *sensor_data_publish;
      itoa(digitalRead(SENSOR), sensor_data_publish, 10);
      client.publish(topic, sensor_data_publish); //publish requires char datatype - digitalRead returns int
      client.loop();
    }
  }
}
