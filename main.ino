#include <WiFiClientSecure.h>
#include <ArduinoJson.h> //You can use the latest version
#include <GxEPD2_BW.h>

#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

const char* ssid     = "YOUR-SSID";     // your network SSID (name of wifi network)
const char* password = "YOUR-PASS"; // your network password

const char* server = "room77.raspiblitz.com";  // Server URL
String host = "https://room77.raspiblitz.com:8077";

String readmacaroon = "YOUR-READ-MACAROON-AS-A_STRING";
String line = "";

const char* test_root_ca =   //SSL must be in this format, SSL for the node can be exported from yournode.com:8077 in firefox
     "-----BEGIN CERTIFICATE-----\n" \
    "MIICBTCCAaqgAwIBAgIQBSMZ9g3niBo1jyzK1DvECDAKBggqhkjOPQQDAjAyMR8w\n" \
    "HQYDVQQKExZsbmQgYXV0b2dlbmVyYXRlZCBjZXJ0MQ8wDQYDVQQDEwZSb29tNzcw\n" \
    "HhcNMTkwNTE2MjAwODUwWhcNMjAwNzEwMjAwODUwWjAyMR8wHQYDVQQKExZsbmQg\n" \
    "YXV0b2dlbmVyYXRlZCBjZXJ0MQ8wDQYDVQQDEwZSb29tNzcwWTATBgcqhkjOPQIB\n" \
    "BggqhkjOPQMBBwasdf;ijasbdf;adbf;akdfbas;dfbvasdfpqwQoTUeV6FVKbFC\n" \
    "CC+fVGRQsXJx+GVUknnNEcJTt/fQ9CmM6stqGPjAo4GhMIGeMA4GA1UdDwEB/wQE\n" \
    "AwICpDAPBgNVHRMBAf8EBTADAQH/MHsGA1UdEQR0MHKCBlJvb203N4IJbG9jYWxo\n" \
    "b3N0ghVyb29tNzcucmFzcGlibGl0ei5jb22CBHVuaXiCCnVuaXhwYWNrZXSHBH8A\n" \
    "AAGHEAAAAAAAADLIHFWQF====ewfqegfiqerogibfeqwdvllsdWZZHfUV0qHBAAA\n" \
    "AAAwCgYIKoZIzj0EAwIDSQAwRgIhALKz3oScii3i+5ltMVQc9u2O38rgfnGCj5Lh\n" \
    "u9iwcAiZAiEA0BjRcisPUlG+SE/s+x6/A2NuT0gtIZ3PKd/GuM5T0jM=\n" \
    "-----END CERTIFICATE-----\n";

WiFiClientSecure client;

void setup() {

  Serial.begin(115200);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  client.setCACert(test_root_ca);
  
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 8077))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    client.println("GET " + host + "/v1/getinfo HTTP/1.0");
    client.println("Grpc-Metadata-macaroon:" + readmacaroon );
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      line = client.readStringUntil('\n');
      Serial.println(line);

      if (line == "\r") {
       Serial.println(line);
        Serial.println("headers received");
        break;
      }
    }

    String content = client.readStringUntil('\n');

    client.stop();

    
     const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(12) + 500;
     DynamicJsonDocument doc(capacity);

    deserializeJson(doc, content);
     

    String alias = doc["alias"]; // Name of node
    String num_active_channels = doc["num_active_channels"]; 
    String num_inactive_channels = doc["num_inactive_channels"]; 
    String num_peers = doc["num_peers"]; 
    String block_height = doc["block_height"]; 
    String synced_to_chain = doc["synced_to_chain"];




display.init(115200);

    
    display.firstPage();
    do
      {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeSansBold18pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(0, 40);
        
        display.println(alias);
        
        display.setFont(&FreeSansBold9pt7b);
        display.println("");
        display.println("Channels: " + num_active_channels + "/-" + num_inactive_channels );

        display.println("Peers: " +  num_peers );

        display.println("Block height: " + block_height);

        display.println("Synced: " + synced_to_chain);
        
      }
     while (display.nextPage());{
      }
      //send the ESP32 to sleep for a while to save power 0.04mah, useful for battery use
     int secs = 60;
     int microsecs = 1000000;
     esp_sleep_enable_timer_wakeup(secs * microsecs);
     esp_deep_sleep_start();

   
  }
}

void loop() {


}
