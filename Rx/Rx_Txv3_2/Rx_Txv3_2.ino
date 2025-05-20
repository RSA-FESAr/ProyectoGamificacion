#include <esp_now.h>
#include <WiFi.h>
#include <Bounce2.h>
#define DEBOUNCE_PIN 16
//Objeto de la biblioteca antirebote
Bounce2::Button button = Bounce2::Button();

// Dirección de MAC de la tarjeta a la que se enviaran los datos
uint8_t broadcastAddress[] = { 0xC8, 0x2E, 0x18, 0xF1, 0x5E, 0x04 };

// Variables globales
bool envia = false;     // Controla si se deben enviar datos
bool inicio = false;    // Indica si se recibió un nuevo dato
bool vacia = false;     // Controla si ya se vació la cadena
bool unico = true;      // Asegura que una cadena se envíe solo una vez
bool prueba = true;     // Bandera para indicador visual en modo gMod
bool cambio = true;     // Permite cambiar indicador al presionar el botón
bool gMod = false;      // Detecta si se activó el modo gMod
String cadena = "";     // Cadena con caracteres únicos

// Estructura para recibir datos
typedef struct struct_message {
  int id;
  char x;
} struct_message;

struct_message myData;

// Estructura para enviar datos
typedef struct envia_datos {
  int id;
  String p;
  char s;
} envia_datos;

// Estructura para almacenar datos a enviar
envia_datos misDatos;

// Variables para almacenar datos de diferentes placas
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;
struct_message board5;
struct_message board6;
struct_message board7;
struct_message board8;
struct_message board9;
struct_message board10;

// Arreglo con todas las estructuras de las placas
struct_message boardsStruct[10] = { board1, board2, board3, board4, board5, board6, board7, board8, board9, board10 };

// Información del peer para ESP-NOW
esp_now_peer_info_t peerInfo;

// Función de callback para cuando los datos son enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Función de callback para cuando se reciben datos
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  boardsStruct[myData.id - 1].x = myData.x;
  inicio = true;
  if (cadena.length() == 0) {
    cadena += boardsStruct[myData.id - 1].x;
  } else {
    if (cadena.indexOf(boardsStruct[myData.id - 1].x) < 0) {
      cadena += boardsStruct[myData.id - 1].x;
      unico = true;
    }
  }
  Serial.println(cadena);
}

// Configura el entorno inicial
void setup() {
  Serial.begin(115200);  // Inicializa el monitor serial
  button.attach(DEBOUNCE_PIN, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  button.interval(5);
  button.setPressedState(LOW);
  WiFi.mode(WIFI_STA);   // Configura el ESP como estación Wi-Fi
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);  // Registra el callback para el envío de datos

  // Configura el peer para ESP-NOW
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);  // Registra el callback para la recepción de datos
  controlaIndicador('r');
}

// Bucle principal
void loop() {
  gMod = digitalRead(4);
  delay(5);
  if(!gMod){
    button.update();
    if ( button.pressed() ) {
        envia = !envia;
    }

    if (cambio){
      if(envia){
        controlaIndicador('g');
      }else{
        controlaIndicador('r');
      }
      cambio = false;
    }

    
    if (envia && !vacia) {
      controlaIndicador('g');
      vacia = true;
    }

    if (inicio) {
      misDatos.id = 0;      // ID de los datos a enviar
      misDatos.p = cadena;  // Datos a enviar
      if (envia&&unico) {
        enviaCadena();
        unico = false;
      }
      inicio = false;
    } else {
      if (!envia) {
        if (vacia) {
          controlaIndicador('r');
          vacia = false;
        }
      }
    }

    prueba= true;

  }else{

    if(prueba){
      controlaIndicador('b');
      prueba = false;
    }

    cambio = true;

    if (inicio) {
      misDatos.id = 0;      // ID de los datos a enviar
      misDatos.p = cadena;  // Datos a enviar
      enviaCadena();
      cadena ="";
      inicio = false;
    }
  }
  delay(200);  // Espera antes de la siguiente iteración
}

void controlaIndicador(char color) {
  unico = true; 
  cadena = "";         // Limpia la cadena
  misDatos.id = 0;
  misDatos.s = color;  // Establece el estado de los datos como 'r'
  misDatos.p = cadena;
  delay(10);  // Espera para asegurar que el mensaje se envíe correctamente
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&misDatos, sizeof(misDatos));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

void enviaCadena() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&misDatos, sizeof(misDatos));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}
