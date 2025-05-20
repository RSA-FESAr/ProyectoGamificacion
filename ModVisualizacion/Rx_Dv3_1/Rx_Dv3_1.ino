#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>

// Configuración de las tiras LED
#define NUM_LEDS 32  // Cambia esto al número de LEDs en cada tira, aquí corresponde a 32
#define NUM_LEDS2 24
#define DATA_PIN_1 25
#define DATA_PIN_2 33
#define DATA_PIN_3 32
#define DATA_PIN_4 26
#define DATA_PIN_5 27


CRGB leds_1[NUM_LEDS];
CRGB leds_2[NUM_LEDS];
CRGB leds_3[NUM_LEDS];
CRGB leds_4[NUM_LEDS2];
CRGB leds_5[NUM_LEDS2];

// Arreglo de valores binarios para los números 0-9
uint32_t valoresBinarios2[10] = {
  0b00011111111111101111111111110000, // 0
  0b00011110000000000000000011110000, // 1
  0b00000001111111100000111111111111, // 2
  0b00011111111000000000111111111111, // 3
  0b00011110000000001111000011111111, // 4
  0b00011111111000001111111100001111, // 5
  0b00011111111111101111111100001111, // 6
  0b00011110000000000000111111110000, // 7
  0b00011111111111101111111111111111, // 8
  0b00011111111000001111111111111111  // 9
};

// Estructura para recibir datos
typedef struct envia_datos {
  int id;
  String p;
  char s;
} envia_datos;

envia_datos misDatos;
bool newData;
String recData;
char char1;
char char2;
char char3;
char tira;

// Callback para recibir datos
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&misDatos, incomingData, sizeof(misDatos));
  Serial.printf("Board ID %u: %u bytes\n", misDatos.id, len);
    recData = misDatos.p;
    newData = true;
    tira = misDatos.s;
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  tira = 'r';
  FastLED.addLeds<NEOPIXEL, DATA_PIN_1>(leds_1, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_2>(leds_2, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_3>(leds_3, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_4>(leds_4, NUM_LEDS2);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_5>(leds_5, NUM_LEDS2);
}

void loop() {
  
  if(newData==true) {
    if(recData.length()<3){
      while(recData.length()!=3){
        recData +='k'; 
      }
    }
    char1 = recData.charAt(0);
    char2 = recData.charAt(1);
    char3 = recData.charAt(2);

    Serial.println(char1);
    Serial.println(char2);
    Serial.println(char3);
    Serial.print("Display: ");
    Serial.println(tira);
    

    mostrarEnDisplay(char1, leds_1);
    mostrarEnDisplay(char2, leds_2);
    mostrarEnDisplay(char3, leds_3);
    colorTira(tira, leds_4,leds_5);
    newData = false;
  }
  delay(1000);  
}

void mostrarEnDisplay(char letra, CRGB ledsD[]) {
  int numero = letra - 'a';  // Convertir 'a'-'j' a 0-9
  if (numero >= 0 && numero <= 9) {
    uint32_t valorBinario = valoresBinarios2[numero];
    for (int i = 0; i < NUM_LEDS; i++) {
      if (bitRead(valorBinario, i)) {
        ledsD[i] = CRGB::White;  // Puedes ajustar el color
      } else {
        ledsD[i] = CRGB::Black;
      }
    }
  }else{
    for (int i = 0; i < NUM_LEDS; i++) {
      ledsD[i] = CRGB::Black;
    }
   Serial.println("Display apagado");
  }
  FastLED.show();
}

void colorTira(char color, CRGB ledsT1[],CRGB ledsT2[]) {
  if (color == 'g') {
    for (int i = 0; i < NUM_LEDS2; i++) {
        ledsT1[i] = CRGB::Green;  // Puedes ajustar el color
        ledsT2[i] = CRGB::Green;
    }
    Serial.println("Verde");
  }else if(color == 'b'){
    for (int i = 0; i < NUM_LEDS2; i++) {
      ledsT1[i] = CRGB::Blue;
      ledsT2[i] = CRGB::Blue;
    }
    Serial.println("Azul");
  }else{
    for (int i = 0; i < NUM_LEDS2; i++) {
      ledsT1[i] = CRGB::Red;
      ledsT2[i] = CRGB::Red;
    }
    Serial.println("Rojo");
  }
  FastLED.show();
}

