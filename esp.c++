//Incluir bibliotecas
#include <DHTesp.h>
#include <EspMQTTClient.h>

#define IR 0
#define IRR 2

int ENA = 14;

int IN1 = 5;

int IN2 = 16;

//porta 4 e 5

bool ir;
bool ir2;

//Para saber mais sobre esta biblioteca, acessar https://github.com/plapointe6/EspMQTTClient

//Definicoes e constantes
char SSIDName[] = ""; //nome da rede WiFi
char SSIDPass[] = ""; //senha da rede WiFI

const int DHT_PIN = 4; //terminal do sensor de temperatura e umidade

char BrokerURL[] = "broker.hivemq.com"; //URL do broker MQTT
char BrokerUserName[] = ""; //nome do usuario para autenticar no broker MQTT
char BrokerPassword[] = ""; //senha para autenticar no broker MQTT
char MQTTClientName[] = "iotmack-grupo06"; //nome do cliente MQTT
int BrokerPort = 1883; //porta do broker MQTT

String TopicoPrefixo = "grupo06-iotmack"; //prefixo do topico
String Topico_01 = TopicoPrefixo+"/Temperatura"; //nome do topico 01
String Topico_02 = TopicoPrefixo+"/Umidade"; //nome do topico 02
String Topico_03 = TopicoPrefixo+"/Volume"; //nome do topico 03
String Topico_04 = TopicoPrefixo+"/Fan"; //nome do topico 04

//Variaveis globais e objetos
DHTesp dhtSensor; //instancia o objeto dhtSensor a partir da classa DHTesp

EspMQTTClient clienteMQTT(SSIDName, SSIDPass, BrokerURL, BrokerUserName, BrokerPassword, MQTTClientName, BrokerPort); //inicializa o cliente MQTT

//Este prototipo de funcao deve estar sempre presente
void onConnectionEstablished() {
}

//Setup
void setup() {
  Serial.begin(9600);

  pinMode(IR,INPUT);
  pinMode(IRR,INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT11); //inicializa o sensor de temperatura e umidade

  clienteMQTT.enableDebuggingMessages(); //habilita mensagens de debug no monitor serial
  //clienteMQTT.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  //clienteMQTT.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  //clienteMQTT.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
}

void enviarDados() {
  TempAndHumidity temp_umid = dhtSensor.getTempAndHumidity(); //instancia o objeto temp_umid a partir da classe TempAndHumidity

  clienteMQTT.publish(Topico_01, String(temp_umid.temperature, 0)); 
  clienteMQTT.publish(Topico_02, String(temp_umid.humidity, 0));

  

  ir = digitalRead(IR);
  ir2 = digitalRead(IRR);
  digitalWrite(ENA, HIGH);

  if ((ir == 0) && (ir2 == 0)){
    clienteMQTT.publish(Topico_03, String(100));

  }else if((ir == 0) && (ir2 == 1)){
    clienteMQTT.publish(Topico_03, String(50));

  }else if((ir == 1) && (ir2 == 1)){
    clienteMQTT.publish(Topico_03, String(0));

  }else{
    clienteMQTT.publish(Topico_03, String(-1));
  }

  if(temp_umid.temperature >= 27){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    clienteMQTT.publish(Topico_04, String(1));
  }else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    clienteMQTT.publish(Topico_04, String(0));
  }

  //Serial.println("Temperatura: " + String(temp_umid.temperature, 2) + "°C");
  //Serial.println("Umidade: " + String(temp_umid.humidity, 1) + "%");
  //Serial.println("---");

}

//Loop
void loop() {

  //clienteMQTT.enableMQTTPersistence(); //estabelece uma conexao persistente
  clienteMQTT.loop(); //funcao necessaria para manter a conexao com o broker MQTT ativa e coletar as mensagens recebidas
  enviarDados(); //funcao para publicar os dados no broker MQTT


  if (clienteMQTT.isWifiConnected() == 1) {
    Serial.println("Conectado ao WiFi!");
  } else {
    Serial.println("Nao conectado ao WiFi!");
  }

  if (clienteMQTT.isMqttConnected() == 1) {
    Serial.println("Conectado ao broker MQTT!");
  } else {
    Serial.println("Nao conectado ao broker MQTT!");
  }

  Serial.println("Nome do cliente: " + String(clienteMQTT.getMqttClientName())
    + " / Broker MQTT: " + String(clienteMQTT.getMqttServerIp())
    + " / Porta: " + String(clienteMQTT.getMqttServerPort())
  );

  delay(5000);
}