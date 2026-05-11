#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// =====================================================
// ACCESS POINT
// =====================================================

const char* ssidAP     = "ESP32-ROBO";
const char* passwordAP = "12345678";

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

DNSServer dnsServer;
WebServer server(80);

// =====================================================
// SERVOS
// =====================================================

#define DEDAO     13
#define INDICADOR 14
#define MEIO      27
#define ANELAR    26
#define MINDINHO  25

#define NUM_SERVOS 5

const int servos[NUM_SERVOS] = {
  DEDAO,
  INDICADOR,
  MEIO,
  ANELAR,
  MINDINHO
};

// =====================================================
// INDICES
// =====================================================

#define IDX_DEDAO     0
#define IDX_INDICADOR 1
#define IDX_MEIO      2
#define IDX_ANELAR    3
#define IDX_MINDINHO  4

// =====================================================
// PWM
// =====================================================

const int PWM_FREQ       = 50;
const int PWM_RESOLUCAO  = 12;

// =====================================================
// AJUSTES
// =====================================================

// QUANTO OS DEDOS FECHAM
int POSICAO_FECHADA = 90;

// VELOCIDADE
// 1 = MUITO RAPIDO
// 3 = NORMAL
// 8 = LENTO
int VELOCIDADE_DEDOS = 2;

// TAMANHO DO PASSO
// MAIOR = MAIS RAPIDO
int PASSO_SERVO = 4;

// =====================================================
// POSICAO ATUAL
// =====================================================

int posAtual[NUM_SERVOS] = {0,0,0,0,0};

// =====================================================
// HTML
// =====================================================

const char PAGINA_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="pt-BR">

<head>

<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Controle da Mao</title>

<style>

*{
  box-sizing:border-box;
  margin:0;
  padding:0;
}

body{
  font-family:Arial,sans-serif;
  background:#0d0d0d;
  color:#ccc;
  padding:20px;
  max-width:500px;
  margin:auto;
}

h1{
  font-size:24px;
  color:white;
  margin-bottom:5px;
}

.subtitle{
  color:#666;
  margin-bottom:20px;
  font-size:12px;
}

.section{
  color:#666;
  font-size:11px;
  margin-top:20px;
  margin-bottom:10px;
  text-transform:uppercase;
}

button{
  width:100%;
  padding:14px;
  border:none;
  border-radius:10px;
  margin-bottom:8px;
  font-size:14px;
  font-weight:bold;
  cursor:pointer;
}

.btn-open{
  background:#00e5a0;
  color:black;
}

.btn-close{
  background:#1e1e1e;
  color:white;
}

.btn-finger{
  background:#181818;
  color:#ccc;
  border:1px solid #2a2a2a;
}

.btn-move{
  background:#26184d;
  color:#c5b2ff;
}

.btn-reset{
  background:#330011;
  color:#ff6688;
}

.grid3{
  display:grid;
  grid-template-columns:1fr 1fr 1fr;
  gap:6px;
}

.grid2{
  display:grid;
  grid-template-columns:1fr 1fr;
  gap:6px;
}

.info{
  display:flex;
  gap:8px;
  margin-bottom:20px;
}

.card{
  flex:1;
  background:#181818;
  border-radius:10px;
  padding:10px;
  text-align:center;
}

.card span{
  display:block;
  color:#666;
  font-size:11px;
}

.card b{
  color:#00e5a0;
  font-size:20px;
}

hr{
  border:none;
  border-top:1px solid #222;
  margin:18px 0;
}

</style>

</head>

<body>

<h1>🤖 Controle da Mao</h1>

<p class="subtitle">
ESP32-ROBO • 192.168.4.1
</p>

<div class="info">

<div class="card">
<span>Velocidade</span>
<b>%VEL%</b>
</div>

<div class="card">
<span>Fechamento</span>
<b>%FEC%</b>
</div>

</div>

<div class="section">
Controle Geral
</div>

<button class="btn-open" onclick="cmd('/abrir')">
Abrir Mao
</button>

<button class="btn-close" onclick="cmd('/fechar')">
Fechar Mao
</button>

<hr>

<div class="section">
Dedos Individuais
</div>

<div class="grid3">

<button class="btn-finger" onclick="cmd('/dedao')">
Polegar
</button>

<button class="btn-finger" onclick="cmd('/indicador')">
Indicador
</button>

<button class="btn-finger" onclick="cmd('/meio')">
Meio
</button>

</div>

<div class="grid2">

<button class="btn-finger" onclick="cmd('/anelar')">
Anelar
</button>

<button class="btn-finger" onclick="cmd('/mindinho')">
Mindinho
</button>

</div>

<hr>

<div class="section">
Movimentos
</div>

<button class="btn-move" onclick="cmd('/rock')">
🤘 Rock
</button>

<button class="btn-move" onclick="cmd('/paz')">
✌️ Paz e Amor
</button>

<button class="btn-move" onclick="cmd('/hang')">
🤙 Hang Loose
</button>

<button class="btn-move" onclick="cmd('/joia')">
👍 Joinha
</button>

<button class="btn-move" onclick="cmd('/contagem')">
🔢 Contagem 0-5
</button>

<button class="btn-move" onclick="cmd('/sequencial')">
✊ Fechar Sequencial
</button>

<hr>

<button class="btn-reset" onclick="cmd('/reset')">
Resetar Posicao
</button>

<script>

function cmd(url){
  fetch(url)
  .catch(err => console.log(err));
}

</script>

</body>
</html>

)rawliteral";

// =====================================================
// PWM
// =====================================================

void moverServo(int pino, int pos){

  pos = constrain(pos, 0, 180);

  int duty = map(pos, 0, 180, 102, 512);

  ledcWrite(pino, duty);
}

// =====================================================
// MOVE TODOS AO MESMO TEMPO
// =====================================================

void moverTodos(int p1, int p2, int p3, int p4, int p5){

  int alvo[NUM_SERVOS] = {p1,p2,p3,p4,p5};

  bool terminou = false;

  while(!terminou){

    terminou = true;

    for(int i=0;i<NUM_SERVOS;i++){

      if(posAtual[i] < alvo[i]){

        posAtual[i] += PASSO_SERVO;

        if(posAtual[i] > alvo[i]){
          posAtual[i] = alvo[i];
        }

        moverServo(servos[i], posAtual[i]);

        terminou = false;
      }

      else if(posAtual[i] > alvo[i]){

        posAtual[i] -= PASSO_SERVO;

        if(posAtual[i] < alvo[i]){
          posAtual[i] = alvo[i];
        }

        moverServo(servos[i], posAtual[i]);

        terminou = false;
      }
    }

    yield();
    delay(VELOCIDADE_DEDOS);
  }
}

// =====================================================
// CONTROLES
// =====================================================

void abrirTodos(){

  moverTodos(
    0,
    0,
    0,
    0,
    0
  );
}

void fecharTodos(){

  moverTodos(
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

void toggleDedo(int idx){

  if(posAtual[idx] == 0){

    moverTodos(
      idx == IDX_DEDAO ? POSICAO_FECHADA : posAtual[IDX_DEDAO],
      idx == IDX_INDICADOR ? POSICAO_FECHADA : posAtual[IDX_INDICADOR],
      idx == IDX_MEIO ? POSICAO_FECHADA : posAtual[IDX_MEIO],
      idx == IDX_ANELAR ? POSICAO_FECHADA : posAtual[IDX_ANELAR],
      idx == IDX_MINDINHO ? POSICAO_FECHADA : posAtual[IDX_MINDINHO]
    );

  }else{

    moverTodos(
      idx == IDX_DEDAO ? 0 : posAtual[IDX_DEDAO],
      idx == IDX_INDICADOR ? 0 : posAtual[IDX_INDICADOR],
      idx == IDX_MEIO ? 0 : posAtual[IDX_MEIO],
      idx == IDX_ANELAR ? 0 : posAtual[IDX_ANELAR],
      idx == IDX_MINDINHO ? 0 : posAtual[IDX_MINDINHO]
    );
  }
}

// =====================================================
// MOVIMENTOS
// =====================================================

// ROCK 🤘
void sinalRock(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    0
  );
}

// PAZ E AMOR ✌️
void pazEAmor(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    0,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

// HANG LOOSE 🤙
void hangLoose(){

  moverTodos(
    0,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    0
  );
}

// JOINHA 👍
void joia(){

  moverTodos(
    0,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

// SEQUENCIAL
void fecharSequencial(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    0,
    0,
    0
  );

  delay(120);

  moverTodos(
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    0,
    0,
    0
  );

  delay(120);

  moverTodos(
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    0,
    0
  );

  delay(120);

  moverTodos(
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    0
  );

  delay(120);

  moverTodos(
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

// =====================================================
// CONTAGEM
// =====================================================

void numero0(){

  moverTodos(
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

void numero1(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    POSICAO_FECHADA,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

void numero2(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    0,
    POSICAO_FECHADA,
    POSICAO_FECHADA
  );
}

void numero3(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    0,
    0,
    POSICAO_FECHADA
  );
}

void numero4(){

  moverTodos(
    POSICAO_FECHADA,
    0,
    0,
    0,
    0
  );
}

void numero5(){

  moverTodos(
    0,
    0,
    0,
    0,
    0
  );
}

void contagem(){

  numero0();
  delay(350);

  numero1();
  delay(350);

  numero2();
  delay(350);

  numero3();
  delay(350);

  numero4();
  delay(350);

  numero5();
}

// =====================================================
// RESET
// =====================================================

void zerarTodos(){

  abrirTodos();
}

// =====================================================
// PAGINA
// =====================================================

void handleRoot(){

  String pagina = FPSTR(PAGINA_HTML);

  pagina.replace("%VEL%", String(VELOCIDADE_DEDOS));
  pagina.replace("%FEC%", String(POSICAO_FECHADA));

  server.send(200,"text/html",pagina);
}

// =====================================================
// RESPOSTA
// =====================================================

void ok(){
  server.send(200,"text/plain","OK");
}

// =====================================================
// ROTAS
// =====================================================

void rotaAbrir(){
  ok();
  abrirTodos();
}

void rotaFechar(){
  ok();
  fecharTodos();
}

void rotaDedao(){
  ok();
  toggleDedo(IDX_DEDAO);
}

void rotaIndicador(){
  ok();
  toggleDedo(IDX_INDICADOR);
}

void rotaMeio(){
  ok();
  toggleDedo(IDX_MEIO);
}

void rotaAnelar(){
  ok();
  toggleDedo(IDX_ANELAR);
}

void rotaMindinho(){
  ok();
  toggleDedo(IDX_MINDINHO);
}

void rotaRock(){
  ok();
  sinalRock();
}

void rotaPaz(){
  ok();
  pazEAmor();
}

void rotaHang(){
  ok();
  hangLoose();
}

void rotaJoia(){
  ok();
  joia();
}

void rotaSequencial(){
  ok();
  fecharSequencial();
}

void rotaContagem(){
  ok();
  contagem();
}

void rotaReset(){
  ok();
  zerarTodos();
}

// =====================================================
// SETUP
// =====================================================

void setup(){

  Serial.begin(115200);

  // ACCESS POINT

  WiFi.softAPConfig(local_ip,gateway,subnet);
  WiFi.softAP(ssidAP,passwordAP);

  Serial.println("\nWiFi criado!");
  Serial.print("SSID: ");
  Serial.println(ssidAP);

  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // DNS

  dnsServer.start(53,"*",local_ip);

  // PWM

  for(int i=0;i<NUM_SERVOS;i++){

    ledcAttach(servos[i],PWM_FREQ,PWM_RESOLUCAO);

    moverServo(servos[i],0);
  }

  // ROTAS

  server.on("/",handleRoot);

  server.on("/abrir",rotaAbrir);
  server.on("/fechar",rotaFechar);

  server.on("/dedao",rotaDedao);
  server.on("/indicador",rotaIndicador);
  server.on("/meio",rotaMeio);
  server.on("/anelar",rotaAnelar);
  server.on("/mindinho",rotaMindinho);

  server.on("/rock",rotaRock);
  server.on("/paz",rotaPaz);
  server.on("/hang",rotaHang);
  server.on("/joia",rotaJoia);

  server.on("/sequencial",rotaSequencial);

  server.on("/contagem",rotaContagem);

  server.on("/reset",rotaReset);

  server.onNotFound(handleRoot);

  server.begin();

  Serial.println("Servidor iniciado!");
}

// =====================================================
// LOOP
// =====================================================

void loop(){

  dnsServer.processNextRequest();

  server.handleClient();
}