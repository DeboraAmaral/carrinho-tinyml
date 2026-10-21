# 🤖 Projeto Carrinho-Robô WALL-E: Controle Web + TinyML (CP1 + CP2)

Repositório oficial contendo o desenvolvimento do carrinho-robô **WALL-E**, desde a montagem física, controle remoto sem fio via WebServer e acionamento de motores (**CP1**), até a implementação do modelo de Inteligência Artificial embarcada (TinyML) utilizando a plataforma **Edge Impulse** e o microcontrolador **ESP32** (**CP2**).

---

## 👥 Integrantes do Grupo
* **Débora da Silva Amaral** — RM: 550412
* **Eduardo Pielich** — RM: 99767
* **Lívia Namba Seraphim** — RM: 97819

---

## 📂 Estrutura do Repositório

```text
carrinho-walle-tinyml/
│
├── README.md                           <-- Documentação unificada do projeto
│
├── cp01/
│   ├── cad/                            <-- Modelagem 3D / Estrutura mecânica
│   ├── codigo_cp01/                    <-- Código do controle manual e motores
│   └── evidencias_cp01/                <-- Esquema elétrico e fotos da montagem
│
└── cp02/
    ├── dataset/                        <-- Arquivos CSV de coleta do HC-SR04
    ├── evidencias_edge_impulse/        <-- Matriz de confusão, acurácia e testes
    └── codigo_cp02_tinyml/
        └── carrinho_walle_ia.ino       <-- Código final embarcado (ESP32 + Edge Impulse + WebServer)
```

---

## 🛠️ Parte 1: Carrinho-Robô Base (CP01)

### 📌 Mapeamento de Hardware e Conexões
* **Microcontrolador:** ESP32 (Wi-Fi Access Point integrado)
* **Ponte H (L298N):**
  * `IN1`: GPIO 25 | `IN2`: GPIO 26 (Motor Esquerdo)
  * `IN3`: GPIO 27 | `IN4`: GPIO 14 (Motor Direito)
* **Sensor Ultrassônico HC-SR04:**
  * `TRIG`: GPIO 5
  * `ECHO`: GPIO 18
* **Servo Motor (SG90):**
  * `SERVO_PIN`: GPIO 15 (Varredura angular de 30° a 150°)
* **Alimentação:** Bateria integrada para acionamento dos motores e microcontrolador.

### 🕹️ Interface de Controle Sem Fio (Wi-Fi AP)
O ESP32 gera sua própria rede Wi-Fi local (**SSID:** `WALL-E` | **Senha:** `wallE123`). Através do navegador web (`http://192.168.4.1`), é possível controlar o robô por uma interface responsiva contendo:
* **Modo Manual:** Botões direcionais (Frente, Ré, Esquerda, Direita e Parar).
* **Segurança:** Timeout de segurança (`700 ms`) para interrupção automática dos motores em caso de perda de sinal (`TEMPO_SEGURANCA`).
* **Modo Automático:** Desvio básico de obstáculos utilizando a varredura mecânica do servo motor.
* **Leitura Manual:** Botões para teste de distância e rotação do servo.

---

## 🧠 Parte 2: Inteligência Artificial Embarcada - TinyML (CP02)

O **WALL-E** evoluiu do controle convencional para um sistema capaz de realizar inferências de Aprendizado de Máquina diretamente na borda (Edge Computing), processando dados ambientais capturados pelo sensor HC-SR04.

### 🔄 Fluxo Completo de TinyML
$$\text{Sensor HC-SR04} \longrightarrow \text{Coleta CSV} \longrightarrow \text{Edge Impulse} \longrightarrow \text{Treinamento} \longrightarrow \text{Biblioteca C++} \longrightarrow \text{ESP32} \longrightarrow \text{Ação dos Motores}$$

1. **Coleta de Dados e Dataset:**
   * **Sensor Utilizado:** Sensor Ultrassônico HC-SR04.
   * **Processo de Coleta:** Medições contínuas de distância armazenadas em arquivos `.csv` organizados na pasta `/cp02/dataset/`.
   * **Classes Definidas:**
     * `Obstáculo Próximo` ($\le 25\text{ cm}$): Provoca parada imediata e acionamento de rotina de desvio.
     * `Caminho Livre`: Permite a progressão contínua do carrinho.

2. **Projeto e Treinamento no Edge Impulse:**
   * **Biblioteca Gerada:** `livns-project-1_inferencing.h`
   * **Estrutura de Entrada:** Processamento de vetor mono-eixo (`EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME == 1`).
   * **Métricas e Validação:** Treinamento do modelo de classificação e exportação da biblioteca compatível com Arduino IDE / ESP32. Prints de acurácia e matriz de confusão salvos em `/cp02/evidencias_edge_impulse/`.

3. **Integração e Inferência no ESP32 (`/cp02/codigo_cp02_tinyml/carrinho_walle_ia.ino`):**
   * **Acionamento:** Botão dedicado **"Classificar (IA)"** na interface Web (função `handleIA()`).
   * **Preenchimento do Buffer:** Leitura sequencial das amostras do HC-SR04 alimentando o vetor de entrada `features[]` conforme o intervalo `EI_CLASSIFIER_INTERVAL_MS`.
   * **Execução do Classificador:** Chamada da função `run_classifier(&signal, &result, false)`.
   * **Tomada de Decisão:** A classe com maior probabilidade (confiança) determina a parada ou movimentação real do carrinho.

---

## 🚦 Instruções de Uso

1. **Upload do Código:** Garanta que a biblioteca `.zip` gerada pelo Edge Impulse (`livns-project-1_inferencing.h`) esteja instalada na sua Arduino IDE e faça o upload do sketch para o ESP32.
2. **Conexão:** Ligue o carrinho **WALL-E**, conecte seu celular/computador à rede Wi-Fi `WALL-E` (senha `wallE123`).
3. **Acesso:** Abra o navegador e acesse `http://192.168.4.1`.
4. **Executando a IA:** Clique no botão **"Classificar (IA)"** na tela do aplicativo web para executar a inferência de TinyML e visualizar o resultado retornado pelo modelo na tela e no Serial Monitor (`115200 baud`).