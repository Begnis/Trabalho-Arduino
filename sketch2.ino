#include <WiFi.h>
#include <PubSubClient.h>

/* 
 * ESP32 de Coordenação - Missão:
 * 1. Conectar ao WiFi Wokwi-GUEST
 * 2. Conectar ao broker MQTT escola-mesquita.cloud.shiftr.io
 * 3. Assinar tópicos Lab1/# até Lab5/#
 * 4. Processar mensagens LabX/AlunoYY
 * 5. Controlar 5 LEDs (GPIOs 2, 4, 5, 18, 19) baseados na média de atenção
 */

// --- Configurações de Rede ---
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

// --- Configurações MQTT (Baseadas no código fornecido) ---
const char* MQTT_HOST = "escola-mesquita.cloud.shiftr.io";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "escola-mesquita";
const char* MQTT_PASS = "blcAkgBumM26K61q";

// --- Constantes do Sistema ---
const int TOTAL_LABS = 5;
const int TOTAL_ALUNOS = 10;
const float LIMIAR = 0.5f;

// --- Mapeamento de LEDs (GPIOs) ---
const int LED_PINS[TOTAL_LABS] = {2, 4, 5, 18, 19};

// --- Estrutura de Dados ---
// Matriz para armazenar o último valor de cada aluno em cada lab
float atencao[TOTAL_LABS][TOTAL_ALUNOS];
bool chegou[TOTAL_LABS][TOTAL_ALUNOS];

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Inicializa ou limpa a matriz de dados
void limparDados() {
  for (int l = 0; l < TOTAL_LABS; l++) {
    for (int a = 0; a < TOTAL_ALUNOS; a++) {
      atencao[l][a] = 0.0f;
      chegou[l][a] = false;
    }
  }
}

void conectarWiFi() {
  Serial.print("Conectando WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" OK");
}

// Atualiza o LED de um laboratório específico baseado na média
void atualizarLedLab(int labIdx) {
  float soma = 0.0f;
  int qtd = 0;

  for (int a = 0; a < TOTAL_ALUNOS; a++) {
    if (chegou[labIdx][a]) {
      soma += atencao[labIdx][a];
      qtd++;
    }
  }

  if (qtd == 0) return;

  float media = soma / qtd;
  
  // Lógica: Média < 0.5 -> LED aceso (HIGH), Média >= 0.5 -> LED apagado (LOW)
  digitalWrite(LED_PINS[labIdx], media < LIMIAR ? HIGH : LOW);

  Serial.print("Lab");
  Serial.print(labIdx + 1);
  Serial.print(": media=");
  Serial.print(media, 4);
  Serial.print(" amostras=");
  Serial.print(qtd);
  Serial.print(" LED=");
  Serial.println(media < LIMIAR ? "ACESO" : "APAGADO");
}

// Callback MQTT - Processa mensagens no formato LabX/AlunoYY
void onMessage(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  
  // 1. Extrair LabX (ex: Lab3)
  int pLab = t.indexOf("Lab");
  int pBarra = t.indexOf("/");
  if (pLab < 0 || pBarra < 0) return;
  
  int labNum = t.substring(pLab + 3, pBarra).toInt(); // Extrai o número entre 'Lab' e '/'
  
  // 2. Extrair AlunoYY (ex: Aluno07)
  int pAluno = t.indexOf("Aluno");
  if (pAluno < 0) return;
  
  int alunoNum = t.substring(pAluno + 5).toInt(); // Extrai o número após 'Aluno'

  // Validação de limites
  if (labNum < 1 || labNum > TOTAL_LABS || alunoNum < 1 || alunoNum > TOTAL_ALUNOS) {
    return;
  }

  // 3. Converter payload para float
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  float valorAtencao = msg.toFloat();

  // 4. Armazenar dados (convertendo para índice 0)
  int lIdx = labNum - 1;
  int aIdx = alunoNum - 1;
  
  atencao[lIdx][aIdx] = valorAtencao;
  chegou[lIdx][aIdx] = true;

  Serial.print("MQTT Recv: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(valorAtencao);

  // 5. Atualizar apenas o LED do lab afetado
  atualizarLedLab(lIdx);
}

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando MQTT...");
    // Gera ID único para evitar conflitos no broker
    String clientId = "esp32-coord-" + String(random(1000, 9999));
    
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println(" OK");
      
      // Assinar tópicos de todos os laboratórios (Lab1/# até Lab5/#)
      for (int i = 1; i <= TOTAL_LABS; i++) {
        String topico = "Lab" + String(i) + "/#";
        mqttClient.subscribe(topico.c_str());
        Serial.print("Assinado: ");
        Serial.println(topico);
      }
    } else {
      Serial.print(" falhou, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 2s...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configurar os 5 LEDs
  for (int i = 0; i < TOTAL_LABS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  limparDados();
  randomSeed(micros());
  conectarWiFi();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(onMessage);
}

void loop() {
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();
}
