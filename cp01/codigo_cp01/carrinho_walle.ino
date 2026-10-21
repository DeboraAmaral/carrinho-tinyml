
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ===== REDE WI-FI DO WALL-E =====
const char* ssid = "WALL-E";
const char* password = "wallE123";

WebServer server(80);

// ===== HC-SR04 =====
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

// ===== SERVO SG90 =====
const int SERVO_PIN = 15;
Servo sensorServo;

// ===== L298N =====
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 27;
const int IN4 = 14;

// ===== CONFIGURACOES =====
const float DISTANCIA_OBSTACULO = 25.0;

bool modoAutomatico = false;
unsigned long ultimoComando = 0;
const unsigned long TEMPO_SEGURANCA = 700;

String pagina = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>WALL-E Control</title>
<style>
body {
  font-family: Arial, sans-serif;
  text-align: center;
  background: #18212b;
  color: white;
  margin: 0;
  padding: 20px;
}
h1 { color: #ffc928; }
button {
  border: 0;
  border-radius: 16px;
  padding: 20px;
  font-size: 18px;
  font-weight: bold;
  background: #ffc928;
  color: #18212b;
  touch-action: none;
  user-select: none;
}
.dir {
  width: 95px;
  height: 75px;
  font-size: 28px;
}
.stop { background: #ff5959; color: white; }
.auto { background: #63d69a; }
.tools { margin: 12px 4px; padding: 14px; }
.grid {
  display: grid;
  grid-template-columns: 100px 100px 100px;
  gap: 10px;
  justify-content: center;
  margin: 25px auto;
}
#status, #distancia {
  margin: 15px;
  padding: 12px;
  background: #293746;
  border-radius: 10px;
}
</style>
</head>
<body>
<h1>🤖 WALL-E</h1>
<p>Controle do carrinho</p>

<div id="status">Modo manual</div>


<div class="grid">
  <!-- Em cima: DIREITA -->
  <div></div>
  <button class="dir"
    onpointerdown="startMove('R',event)"
    onpointerup="stopMove(event)"
    onpointercancel="stopMove(event)"
    onpointerleave="stopMove(event)">⬆️</button>
  <div></div>

  <!-- Esquerda: RÉ -->
  <button class="dir"
    onpointerdown="startMove('B',event)"
    onpointerup="stopMove(event)"
    onpointercancel="stopMove(event)"
    onpointerleave="stopMove(event)">⬅️</button>

  <!-- Centro: PARAR -->
  <button class="dir stop"
    onpointerdown="stopMove(event)">■</button>

  <!-- Direita: FRENTE -->
  <button class="dir"
    onpointerdown="startMove('F',event)"
    onpointerup="stopMove(event)"
    onpointercancel="stopMove(event)"
    onpointerleave="stopMove(event)">➡️</button>

  <!-- Embaixo: ESQUERDA -->
  <div></div>
  <button class="dir"
    onpointerdown="startMove('L',event)"
    onpointerup="stopMove(event)"
    onpointercancel="stopMove(event)"
    onpointerleave="stopMove(event)">⬇️</button>
  <div></div>
</div>

<button class="tools auto" onclick="toggleAuto()">
  Ligar / desligar automático
</button>
<br>
<button class="tools" onclick="testServo()">Testar servo</button>
<button class="tools" onclick="getDistance()">Medir distância</button>

<div id="distancia">Distância: aguardando leitura</div>

<script>
let movimento = null;
let repeticao = null;

function enviar(comando) {
  fetch('/cmd?m=' + comando).catch(() => {});
}

function startMove(comando, event) {
  if (event) event.preventDefault();
  clearInterval(repeticao);
  movimento = comando;
  enviar(comando);
  repeticao = setInterval(() => enviar(comando), 250);
}

function stopMove(event) {
  if (event) event.preventDefault();
  clearInterval(repeticao);
  repeticao = null;
  movimento = null;
  enviar('S');
}

function toggleAuto() {
  clearInterval(repeticao);
  movimento = null;
  fetch('/auto')
    .then(r => r.text())
    .then(t => document.getElementById('status').innerText = t)
    .catch(() => {});
}

function testServo() {
  fetch('/servo').catch(() => {});
}

function getDistance() {
  fetch('/dist')
    .then(r => r.text())
    .then(t => document.getElementById('distancia').innerText =
      'Distância: ' + t)
    .catch(() => {});
}

window.addEventListener('blur', stopMove);
document.addEventListener('visibilitychange', () => {
  if (document.hidden) stopMove();
});
</script>
</body>
</html>
)rawliteral";

// ===== MOTORES =====
void pararMotores() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void frente() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void re() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void esquerda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void direita() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ===== SENSOR =====
float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duracao = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracao == 0) return -1;

  return duracao * 0.0343 / 2.0;
}

// ===== PAGINA E COMANDOS =====
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", pagina);
}

void handleCommand() {
  if (!server.hasArg("m")) {
    server.send(400, "text/plain", "Comando ausente");
    return;
  }

  String comando = server.arg("m");

  if (comando == "S") {
    modoAutomatico = false;
    pararMotores();
    ultimoComando = millis();
    server.send(200, "text/plain", "Parado");
    return;
  }

  modoAutomatico = false;
  ultimoComando = millis();

  if (comando == "F") frente();
  else if (comando == "B") re();
  else if (comando == "L") esquerda();
  else if (comando == "R") direita();
  else {
    pararMotores();
    server.send(400, "text/plain", "Comando invalido");
    return;
  }

  server.send(200, "text/plain", "OK");
}

void handleAuto() {
  pararMotores();
  modoAutomatico = !modoAutomatico;

  if (modoAutomatico) {
    ultimoComando = millis();
    server.send(200, "text/plain",
                "Modo automatico ligado");
  } else {
    server.send(200, "text/plain",
                "Modo automatico desligado");
  }
}

void handleServo() {
  modoAutomatico = false;
  pararMotores();

  sensorServo.write(30);
  delay(400);
  sensorServo.write(90);
  delay(400);
  sensorServo.write(150);
  delay(400);
  sensorServo.write(90);

  server.send(200, "text/plain", "Servo testado");
}

void handleDistance() {
  float d = medirDistancia();

  if (d < 0) {
    server.send(200, "text/plain",
                "SEM ECO - verifique sensor e fios");
  } else {
    server.send(200, "text/plain",
                String(d, 1) + " cm");
  }
}

// ===== DESVIO AUTOMATICO BASICO =====
void modoDesvio() {
  float centro = medirDistancia();

  if (centro < 0) {
    pararMotores();
    return;
  }

  if (centro > DISTANCIA_OBSTACULO) {
    frente();
    return;
  }

  pararMotores();
  delay(200);

  sensorServo.write(150);
  delay(400);
  float esquerdaDist = medirDistancia();

  sensorServo.write(30);
  delay(400);
  float direitaDist = medirDistancia();

  sensorServo.write(90);
  delay(150);

  if (esquerdaDist < 0 || direitaDist < 0) {
    pararMotores();
    return;
  }

  re();
  delay(250);
  pararMotores();
  delay(100);

  if (esquerdaDist > direitaDist &&
      esquerdaDist > DISTANCIA_OBSTACULO) {
    esquerda();
    delay(350);
  } else if (direitaDist > DISTANCIA_OBSTACULO) {
    direita();
    delay(350);
  }

  pararMotores();
}

// ===== INICIALIZACAO =====
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pararMotores();
  digitalWrite(TRIG_PIN, LOW);

  sensorServo.setPeriodHertz(50);
  sensorServo.attach(SERVO_PIN, 500, 2400);
  sensorServo.write(90);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/cmd", handleCommand);
  server.on("/auto", handleAuto);
  server.on("/servo", handleServo);
  server.on("/dist", handleDistance);
  server.begin();

  Serial.println();
  Serial.println("=== WALL-E WEB ===");
  Serial.print("Wi-Fi: ");
  Serial.println(ssid);
  Serial.print("Senha: ");
  Serial.println(password);
  Serial.print("Acesse: http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("Motores iniciam parados.");
}

void loop() {
  server.handleClient();

  if (modoAutomatico) {
    modoDesvio();
    server.handleClient();
  } else {
    if (millis() - ultimoComando > TEMPO_SEGURANCA) {
      pararMotores();
    }
  }
}
