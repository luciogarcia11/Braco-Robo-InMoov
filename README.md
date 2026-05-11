# Braco-Robo-InMoov

## 🦾 Introdução

Este projeto é o controle de um braço robótico inspirado no modelo InMoov, utilizando um **ESP32** para controlar 5 servomotores que movimentam cada dedo da mão. O controle é feito via interface web responsiva, acessada diretamente pela rede Wi-Fi criada pelo próprio ESP32 — sem necessidade de roteador ou internet.

---

## 📷 Fotos do Projeto

*Em breve*

---

## 🛠 Materiais Utilizados

| Material | Descrição |
|---|---|
| **ESP32** | Placa principal para controle e servidor web |
| **5x Servomotores** | Um para cada dedo da mão |
| **Fonte de alimentação** | Recomendado 5V 3A+ para os servos |
| **Fios jumper** | Para conexão dos componentes |
| **Estrutura InMoov** | Peças impressas em 3D do projeto InMoov |

---

## 🔌 Pinagem

| Dedo | Pino ESP32 |
|---|---|
| Polegar | GPIO 13 |
| Indicador | GPIO 14 |
| Médio | GPIO 27 |
| Anelar | GPIO 26 |
| Mindinho | GPIO 25 |

> **Atenção:** Os servos consomem corrente alta. Não os alimente diretamente pelo pino 3.3V ou 5V do ESP32. Use uma fonte externa de 5V ligada diretamente nos servos, com o GND em comum com o ESP32.

*Foto da pinagem e conexão da fonte — em breve*

---

## 📶 Como Conectar

O ESP32 cria sua própria rede Wi-Fi. Não é necessário roteador nem internet.

1. Ligue o ESP32 (via USB ou fonte)
2. No seu celular ou computador, vá em **Configurações > Wi-Fi**
3. Conecte-se à rede:

```
Nome:  ESP32-ROBO
Senha: 12345678
```

4. Abra o navegador e acesse:

```
http://192.168.4.1
```

A interface de controle abrirá automaticamente.

---

## 📱 Interface Web

A interface é responsiva e funciona bem no celular. Ela possui:

- **Controle geral** — Abrir e fechar a mão inteira
- **Dedos individuais** — Toggle em cada dedo (toque uma vez para fechar, toque novamente para abrir)
- **Movimentos prontos** — Rock, Paz e Amor, Hang Loose, Joinha, Fechar Sequencial, Contagem 0 a 5
- **Reset** — Volta todos os dedos para a posição aberta

---

## ⚙️ Ajustes no Código

No topo do arquivo `ESP32_ROBO.ino` você encontra as variáveis de ajuste:

```cpp
// Quanto os dedos fecham (em graus)
// Teste valores como 40, 60, 90, 120 conforme seu servo e estrutura
int POSICAO_FECHADA = 90;

// Velocidade entre cada passo do movimento
// 1 = muito rápido | 3 = normal | 8 = lento
int VELOCIDADE_DEDOS = 2;

// Tamanho do passo em graus por ciclo
// Maior = mais rápido e menos suave
int PASSO_SERVO = 4;
```

Ajuste esses valores conforme a montagem física do seu braço.

---

## 💻 Estrutura do Repositório

```
Braco-Robo-InMoov/
├── README.md
└── ESP32_ROBO/
    └── ESP32_ROBO.ino
```

---

## 🔧 Como Carregar o Código

1. Instale o [Arduino IDE](https://www.arduino.cc/en/software)
2. Adicione o suporte ao ESP32 em **Arquivo > Preferências > URLs adicionais**:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
3. Instale a placa **ESP32** pelo Gerenciador de Placas
4. Selecione a placa **ESP32 Dev Module**
5. Abra o arquivo `ESP32_ROBO/ESP32_ROBO.ino`
6. Conecte o ESP32 via USB e clique em **Carregar**

---

## 🌟 Futuras Melhorias

- Adicionar controle do pulso e cotovelo
- Integração com sensor de luva para espelhar movimentos da mão humana
- Controle via Bluetooth
- Feedback de posição via interface web em tempo real

---

## 🔗 Links Úteis

- [Projeto InMoov original](https://inmoov.fr)
- [Documentação ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Repositório esp32 para Arduino](https://github.com/espressif/arduino-esp32)

---

## 📄 Licença

Este projeto está licenciado sob os termos da [MIT License](LICENSE).

---

## 👥 Autor

- **Lucio Garcia Leite Filho**