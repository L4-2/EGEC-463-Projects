#include <WiFi.h>

const char* ssid = "esp32-project";
const char* password = "7441163a";
const char* ip = "192.168.0.192";
int status = WL_IDLE_STATUS;

// create client
WiFiClient client;


void setup()
{
    Serial.begin(115200);

    delay(10);

    // connecting to wifi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Connection successful.");
    Serial.println("This board's IP address is: ");
    Serial.println(WiFi.localIP());
    
}

void loop(){

  // connect to specified ip and port
  if(client.connect(ip, 5500)){
    Serial.println("Successfully connected to server");

    delay(2000);

    // send a message
    client.println("Hello from the esp32");
    Serial.println("Sent the message.");

    delay(5000);

    // create a message buffer to hold the received data
    char buffer[256];
    int index = 0;

    // take in the received data by checking if there is available data and using client.read to take in the bytes and input them into the buffer array
    while(client.available() > 0){
      while(client.available()){
        buffer[index] = client.read();
        index++;
      }
    }

    // see that we received the same message from server
    Serial.println("Received from server:");
    Serial.println(buffer);
  }

}
