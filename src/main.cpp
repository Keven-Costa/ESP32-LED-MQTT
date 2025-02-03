#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define LED 18
#define ID_MQTT "k19"

//WiFi
const char* SSID = "NOME DA SUA REDE";      // SSID nome da rede WiFi que deseja se conectar
const char* PASSWORD = "SENHA DA SUA REDE";  // Senha da rede WiFi que deseja se conectar


//MQTT Server
const char* BROKER_MQTT = "URL DO SEU BROKEN";           // URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                                  // Porta do Broker MQTT
const char* MQTT_USER = "NOME DE USUARIO DO BROKEN";     // Substitua pelo seu nome de usuário
const char* MQTT_PASSWORD = "SENHA DO BROKEN";           // Substitua pela sua senha
const char* TOPIC_SUBSCRIBE = "/teste/led/status";

WiFiClient wifiClient; 
PubSubClient MQTT(wifiClient);  

//Declaração das Funções
void mantemConexoes();                                              //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();                                                 //Faz conexão com WiFi
void conectaMQTT();                                                 //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length); // processar as mensagens recebidas.

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  conectaWiFi();

  MQTT.setServer(BROKER_MQTT, BROKER_PORT); // Configura o servidor (broker) MQTT ao qual o ESP32 se conectará.
  MQTT.setCallback(recebePacote);           //Define uma função de callback (recebePacote) que será chamada sempre que o ESP32 receber uma mensagem em um tópico MQTT inscrito.
}

void loop() {
  mantemConexoes();
  MQTT.loop(); // Mantém a conexão e processa mensagens MQTT
}

void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }

  conectaWiFi();  //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD);  // Conecta na rede WI-FI
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    // Adiciona autenticação com nome de usuário e senha
    if (MQTT.connect(ID_MQTT, MQTT_USER, MQTT_PASSWORD)) {
       Serial.println("Conectado ao Broker com sucesso!");
       MQTT.subscribe(TOPIC_SUBSCRIBE); // Inscreva-se nos tópicos aqui, se necessário

    } else {
      Serial.println("Não foi possível se conectar ao broker.");
      Serial.print("Estado de conexão: ");
      Serial.println(MQTT.state());
      Serial.println("Nova tentativa de conexão em 10s");
      delay(10000);

    }
  }
}

void recebePacote(char* topic, byte* payload, unsigned int length) {


  // Verifica o payload sem converter para String
  if (length == 5 && memcmp(payload, "ligar", 5) == 0) {
    digitalWrite(LED, HIGH); // Liga o LED
    Serial.println("LED ligado");

  } else if (length == 8 && memcmp(payload, "desligar", 8) == 0) {
    digitalWrite(LED, LOW); // Desliga o LED
    Serial.println("LED desligado");

  } else {
    Serial.println("Comando desconhecido");
  }
}
