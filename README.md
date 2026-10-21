# 🤖 Projeto Carrinho-Robô: Do Controle Remoto ao TinyML (CP1 + CP2)

Repositório unificado da evolução do projeto de carrinho-robô desenvolvido para a disciplina de Project-based Maker Lab. O projeto abrange desde a fabricação mecânica, eletrônica e controle sem fio (**CP1**), até a implementação de inteligência artificial embarcada com Edge Impulse e ESP32 (**CP2**).

## 👥 Integrantes
* **Nome 1:** [Seu Nome] — RM: [Seu RM]
* **Nome 2:** [Nome do Colega] — RM: [RM do Colega]

---

## 🛠️ Parte 1: Carrinho-Robô Base (CP01)
A primeira etapa consistiu na construção física e lógica do carrinho:
* **Chassi e Projeto Mecânico:** Chassi comercial adaptado com carenagem desenvolvida em acrílico e papelão.
* **Movimentação e Elétrica:** Utilização de ESP32, Ponte H (L298N), motores DC e alimentação por pilha.
* **Controle Sem Fio:** Implementação de controle via Bluetooth e Wi-Fi.
* **Sensor Integrado:** Sensor Ultrassônico HC-SR04 integrado para detecção inicial.
* **Documentação e Código:** Disponíveis na pasta `/cp01/`.

---

## 🧠 Parte 2: Aplicação de TinyML (CP02)
Evolução do projeto incorporando aprendizado de máquina na borda (Edge):
* **Sensor Utilizado:** Ultrassônico HC-SR04 para captura de dados reais.
* **Classes Definidas:** 
  1. *[Ex: Obstáculo Próximo]* ➔ Ação: Parar / Dar ré
  2. *[Ex: Caminho Livre]* ➔ Ação: Avançar
* **Dataset e Coleta:** Dados coletados pelo sensor do próprio carrinho e salvos em formato `.csv` na pasta `/cp02/dataset/`.
* **Edge Impulse:** 
  * Modelo de classificação treinado na plataforma com acurácia de **[X]%**.
  * Evidências e prints salvos em `/cp02/evidencias_edge_impulse/`.
* **Código e Integração:** A biblioteca gerada pelo Edge Impulse foi integrada ao ESP32, permitindo que o resultado da inferência comande diretamente os motores do carrinho. O código está na pasta `/cp02/codigo_cp02_tinyml/`.

---

## 🎥 Demonstração e Vídeos
* **Vídeo de Demonstração (CP1 & CP2):** [Inserir link do YouTube ou Google Drive com o vídeo funcionando]
