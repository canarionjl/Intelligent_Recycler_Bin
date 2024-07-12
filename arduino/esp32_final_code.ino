#include "esp_camera.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <base64.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


#define CAMERA_MODEL_WROVER_KIT

#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#define SERVER_URL "http://192.168.210.195:5000/requestBin"

#define SOUND_SPEED 0.034

int working = 0;

// ------------------------------------ CONFIGURACIÓN DE MONITOR SERIE --------------------------------- //

void iniciar_monitor_serie_1() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
}



// ------------------------------------ CONFIGURACIÓN DE CONEXIÓN WIFI --------------------------------- //

//const char* ssid = "vodafoneAA8TC8";
//const char* password = "zbd3xh97gLaEcYda";

const char* ssid = "POCO X4 GT";
const char* password = "holahola";

void iniciar_conexion_wifi_2() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

}



// ------------------------------------ CONFIGURACIÓN DE CÁMARA --------------------------------- //

camera_config_t config;

void config_camara() {

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 40;
  config.fb_count = 1;

}

void iniciar_camara() {
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void parametros_camara() {
  sensor_t * s = esp_camera_sensor_get();

  if (s->id.PID == OV3660_PID) {
    // s->set_vflip(s, -1); // voltear la camara
    s->set_brightness(s, 1); // ajustando su brillo
    s->set_saturation(s, -2); // ajustando la saturacion
  }
}


void inicializacion_de_camara_3() {
  config_camara();
  iniciar_camara();
  parametros_camara();
}

String sacar_foto() {

  camera_fb_t  * fb = esp_camera_fb_get();

  if (!fb) {
    return "-1";
  }

  String codificado = base64::encode(fb->buf, fb->len);

  esp_camera_fb_return(fb);

  return codificado;
}



// ------------------------------------ CONFIGURACIÓN DE LA PETICIÓN HTTP POST --------------------------------- //


WiFiClient client;
HTTPClient http;


void iniciar_cliente_HTTP_4() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No hay conexión a la red...");
  }
  http.begin(client, SERVER_URL);

}

String jsonBuffer = "";

String realizar_peticion_POST(String imagen) {

  http.addHeader("Content-Type", "application/json");

//  StaticJsonDocument<4096> doc;
//  doc["imagen"] = imagen;
//
//  String jsonBuffer;
//  serializeJson(doc, jsonBuffer);

  jsonBuffer = " {\"imagen\": \""+imagen+"\" }";

  int httpResponseCode = http.POST(jsonBuffer);

  delay(2000);

  String bin = "Error";

  if (httpResponseCode == HTTP_CODE_OK) {
   bin = http.getString();
  }

  http.end();

  return bin;

}

// ------------------------------------ CONFIGURACIÓN DEl SENSOR DE DISTANCIA --------------------------------- //

const int trigPin = 33;
const int echoPin = 32;



void config_distance_sensor_5() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float getDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * SOUND_SPEED / 2;

  return distance;
}



// ------------------------------------ CONFIGURACIÓN DEl LED RGB --------------------------------- //

//const int channelBlue = 0;
//const int channelRed = 1;
//const int channelGreen = 2;
//
//const int ledBlue = 14;
//const int ledRed = 13;
//const int ledGreen = 12;
//
//const int frequency = 50000;
//const int ledResolution = 8;
//
//void config_led_rgb_6() {
//  ledcSetup(channelRed, frequency, ledResolution);
//  ledcSetup(channelGreen, frequency, ledResolution);
//  ledcSetup(channelBlue, frequency, ledResolution);
//  ledcAttachPin(ledRed, channelRed);
//  ledcAttachPin(ledGreen, channelGreen);
//  ledcAttachPin(ledBlue, channelBlue);
//  ledcWrite(ledRed, 0);
//  ledcWrite(ledBlue, 0);
//  ledcWrite(ledGreen, 0);
//}
//
//void set_led_rgb(int red, int green, int blue) {
//
//  ledcWrite(channelRed, red);
//  ledcWrite(channelGreen, green);
//  ledcWrite(channelBlue, blue);
//
//}
//
//void set_color(String color){
//
//  int red = 13;
//  int green = 12;
//  int blue = 14;
//
//  if (color == "Azul") {
//      blue = 255;
//  } else if (color == "Verde") {
//      green = 255;
//  } else if (color == "Marron") {
//      red = 255;
//      green = 35;
//  } else if (color == "Amarillo") {
//      red = 128;
//      green = 128;
//      } else {
//      red = 255;
//  }
//
//  set_led_rgb(red,green,blue);
//
//}
//


// ---------------------------------------- MÉTODOS GENERALES ----------------------------------- //

void setup() {
  iniciar_monitor_serie_1();
  iniciar_conexion_wifi_2();
  inicializacion_de_camara_3();
  iniciar_cliente_HTTP_4();
  config_distance_sensor_5();
  //config_led_rgb_6();
}

void loop() {

  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  float distance = getDistance();

  if (distance < 30.00) {

    if (working == 1) return;

    working = 1;

    String codificado = sacar_foto();

    delay(500);

    String bin = "Error";

    if (codificado != "-1" && codificado != "") {
      bin = realizar_peticion_POST(codificado);
    }

    delay (500);

    // set_color(bin);

    if (bin != "Error"){
      Serial.print("Se ha clasificado un residuo en el contenedor: ");
      Serial.println(bin);
    }
    

  }

  delay(2000);
  working = 0;
}
