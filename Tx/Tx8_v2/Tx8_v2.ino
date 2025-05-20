#define BOUNCE_PIN 16

#include <esp_now.h>
#include <WiFi.h>
#include <Bounce2.h>

Bounce rebote = Bounce();

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = { 0xB0, 0xB2, 0x1C, 0xA7, 0x1D, 0x18 };
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;  // must be unique for each sender board
  char x;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  rebote.attach(BOUNCE_PIN, INPUT);
  WiFi.mode(WIFI_STA);
  rebote.interval(5);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  myData.id = 8;
  myData.x = 'i';
}

void loop() {
  rebote.update();
  if (rebote.changed()) {
    int deboucedInput = rebote.read();
    if (deboucedInput == LOW) {
      enviaChar();
    }
  }
}


void enviaChar() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}