// this is an arduino sketch, run this in the arduino ide with your esp32-pico-d4 or whatever you have...

#include <WiFi.h>

const char* ssid = "esp32-project";
const char* password = "7441163a";
int status = WL_IDLE_STATUS;

// for whatever reason, the esp32 doesn't like any port other than 80, so we're stuck with that
WiFiServer server(80);

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
    
    // start server
    server.begin();
    Serial.println("Server has been started");

}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients


 // Output if client connects
 if(client){
  if(client.connected()){

    // letting serial output know if something connected
    Serial.println("A client has connected.");

    // Flush input buffer, not sure if this is needed...
    client.flush();

    // let the client know they've connected
    client.println("Connected to ESP32 Server.");

    delay(2000);

    // busy loop waiting for data to come in so connection doesn't close
    while(client.available() <= 0){
      Serial.println("Waiting for client to send something...");
      delay(5000);
    }

    // when data is incoming 
    while (client.available() > 0) {
      Serial.println("Receiving Data...");

      // array to store incoming data 
      char message[256];
      int index = 0;

      // input each byte into the array and increase the size, this is to receive data as a string essentially
      while(client.available()){
        message[index] = client.read();
        index++;
      }
       
      // echo back the message to the client
      client.println(message);

      // display the received message in the serial monitor
      Serial.println("Recieved:");
      Serial.println(message);

      // reset the array so as not to keep old data
      memset(message, 0, sizeof(message));
      
      // deadtime so that we wait for any more data
      delay(9000);
    }
  }
 }

}
