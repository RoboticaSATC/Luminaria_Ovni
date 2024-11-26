#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
//#include <Servo.h>

//--------Configurações da rede WiFi (Access Point)--------
const char* ssid = "Robos_Andreia";
const char* password = "12345678";


//--------Configuração do IP estático--------
IPAddress local_IP(192, 168, 4, 1);   // IP do ESP32
IPAddress gateway(192, 168, 4, 1);    // Gateway
IPAddress subnet(255, 255, 255, 0);   // Máscara de sub-rede

//--------Criação do servidor web--------
WebServer server(80);

//--------Definições dos pinos--------
#define PIXEL_PIN1     13
#define PIXEL_PIN2     12
#define SERVO_PIN      15

//--------Configurações iniciais para fita e anel de LED--------
#define PIXEL_COUNT1  9
#define PIXEL_COUNT2  12

Adafruit_NeoPixel strip1(PIXEL_COUNT1, PIXEL_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(PIXEL_COUNT2, PIXEL_PIN2, NEO_GRB + NEO_KHZ800);

//--------Configurações iniciais do servo motor--------
Servo myservo;

//--------Definição das variáveis globais--------
int modo1 = 0;
int modo2 = 0;
int pos = 0;

//--------Protótipos das funções--------
void handleRoot();
void handleNotFound();
void updateLED1(int change);
void updateLED2(int change);
void moveServoTo(int position);

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(9600);

  // Configura o ESP32 como Access Point
  WiFi.softAP(ssid, password);
  
  // Configura o IP estático
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar IP estático");
  }
  
  Serial.println("Access Point configurado");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Inicializa o servidor web
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/led1up", [](){ updateLED1(1); });
  server.on("/led1down", [](){ updateLED1(-1); });
  server.on("/led2up", [](){ updateLED2(1); });
  server.on("/led2down", [](){ updateLED2(-1); });
  server.on("/servo0", [](){ moveServoTo(0); });
  server.on("/servo180", [](){ moveServoTo(180); });
  server.begin();
  Serial.println("Servidor HTTP iniciado");

  // Inicializa o servo motor
  myservo.attach(SERVO_PIN);
  myservo.write(pos);

  // Inicializa a fita de LED
  strip1.begin();
  strip1.show();
  strip1.clear();

  // Inicializa o anel de LED
  strip2.begin();
  strip2.show();
  strip2.clear();
}

void loop() {
  // Processa as requisições HTTP
  server.handleClient();

  // Troca as cores da fita de LED
  switch(modo1) {
    case 0:
      colorWipe1(strip1.Color( 0, 0, 0), 50);        // Desligado
      break;
    case 1:
      colorWipe1(strip1.Color( 0, 0, 205), 50);      // Azul
      break;
    case 2:
      colorWipe1(strip1.Color( 0, 206, 209), 50);    // Ciano
      break;
    case 3:
      colorWipe1(strip1.Color( 0, 128, 0), 50);      // Verde
      break;
    case 4:
      colorWipe1(strip1.Color( 75, 0, 130), 50);     // Roxo
      break;
    case 5:
      colorWipe1(strip1.Color( 255, 105, 180), 50);  // Rosa
      break;
    case 6:
      colorWipe1(strip1.Color( 255, 0, 0), 50);      // Vermelho
      break;
    case 7:
      colorWipe1(strip1.Color( 255, 140, 0), 50);    // Laranja
      break;
    case 8:
      colorWipe1(strip1.Color( 255, 215, 0), 50);    // Amarelo
      break;
    case 9:
      theaterChase1(strip1.Color( 0, 0, 205), 50);      // Azul
      break;
    case 10:
      theaterChase1(strip1.Color( 0, 206, 209), 50);    // Ciano
      break;
    case 11:
      theaterChase1(strip1.Color( 0, 128, 0), 50);      // Verde
      break;
    case 12:
      theaterChase1(strip1.Color( 75, 0, 130), 50);     // Roxo
      break;
    case 13:
      theaterChase1(strip1.Color( 255, 105, 180), 50);  // Rosa
      break;
    case 14:
      theaterChase1(strip1.Color( 255, 0, 0), 50);      // Vermelho
      break;
    case 15:
      theaterChase1(strip1.Color( 255, 140, 0), 50);    // Laranja
      break;
    case 16:
      theaterChase1(strip1.Color( 255, 215, 0), 50);    // Amarelo
      break;
    case 17:
      rainbow1(10);
      break;
  }

  // Troca as cores do anel de LED
  switch(modo2) {
    case 0:
      colorWipe2(strip2.Color( 0, 0, 0), 50);        // Desligado
      break;
    case 1:
      colorWipe2(strip2.Color( 0, 0, 205), 50);      // Azul
      break;
    case 2:
      colorWipe2(strip2.Color( 0, 206, 209), 50);    // Ciano
      break;
    case 3:
      colorWipe2(strip2.Color( 0, 128, 0), 50);      // Verde
      break;
    case 4:
      colorWipe2(strip2.Color( 75, 0, 130), 50);     // Roxo
      break;
    case 5:
      colorWipe2(strip2.Color( 255, 105, 180), 50);  // Rosa
      break;
    case 6:
      colorWipe2(strip2.Color( 255, 0, 0), 50);      // Vermelho
      break;
    case 7:
      colorWipe2(strip2.Color( 255, 140, 0), 50);    // Laranja
      break;
    case 8:
      colorWipe2(strip2.Color( 255, 215, 0), 50);    // Amarelo
      break;
  }
}

//--------Funções do servidor web--------
void handleRoot() {
  String html = "<html><head><title>ESP32 Web Server</title></head><body>";
  html += "<h1>Controle de LEDs e Servo Motor</h1>";
  html += "<h2>LED 1</h2>";
  html += "<button onclick=\"location.href='/led1down'\">Anterior</button>";
  html += "<button onclick=\"location.href='/led1up'\">Próximo</button>";
  html += "<h2>LED 2</h2>";
  html += "<button onclick=\"location.href='/led2down'\">Anterior</button>";
  html += "<button onclick=\"location.href='/led2up'\">Próximo</button>";
  html += "<h2>Servo Motor</h2>";
  html += "<button onclick=\"location.href='/servo0'\">Mover para 0°</button>";
  html += "<button onclick=\"location.href='/servo180'\">Mover para 180°</button>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

void updateLED1(int change) {
  modo1 = (modo1 + change + 18) % 18;
  handleRoot();  // Recarrega a página original após a mudança
}

void updateLED2(int change) {
  modo2 = (modo2 + change + 9) % 9;
  handleRoot();  // Recarrega a página original após a mudança
}

void moveServoTo(int position) {
  pos = position;
  myservo.write(pos);
  delay(500);  // Aguarde meio segundo para o servo se mover
  handleRoot();  // Recarrega a página original após o movimento
}

//--------Funções de controle do anel e fita de LED--------
void colorWipe1(uint32_t color, int wait) {
  for (int i = 0; i < strip1.numPixels(); i++) {
    strip1.setPixelColor(i, color);
    strip1.show();
    delay(wait);
  }
}

void theaterChase1(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 3; b++) {
      strip1.clear();
      for (int c = b; c < strip1.numPixels(); c += 3) {
        strip1.setPixelColor(c, color);
      }
      strip1.show();
      delay(wait);
    }
  }
}

void rainbow1(int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 3 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip1.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip1.numPixels());
      strip1.setPixelColor(i, strip1.gamma32(strip1.ColorHSV(pixelHue)));
    }
    strip1.show();
    delay(wait);
  }
}

void colorWipe2(uint32_t color, int wait) {
  for (int i = 0; i < strip2.numPixels(); i++) {
    strip2.setPixelColor(i, color);
    strip2.show();
    delay(wait);
  }
}