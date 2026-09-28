#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // se a tela nao mostrar nada, teste 0x3F
SoftwareSerial softserial(11,12);   // RX = 11 (vem do TX do DFPlayer), TX = 12 (vai ao RX do DFPlayer)
DFRobotDFPlayerMini dfplayer;

String mensagem = "";
int qtd_sons = 8;                   // usado se o DFPlayer nao informar quantos arquivos ha no SD
const int somNotificacao = 8;       // som tocado quando chega mensagem pela serial

const int pinoAmarelo = 4;          // proximo SFX
const int pinoVermelho = 2;         // SFX anterior
const int pinoPreto = 6;            // tocar SFX selecionado
const int pinoLED = 8;
const int pinoVolume = A0;
const int pinoBusy = 3;             // LOW enquanto o DFPlayer esta tocando

unsigned long tempoAnterior = 0;
unsigned long tempoBotao = 0;

int arquivoSelecionado = 1;
bool browseStatus = false;
bool amareloAnterior = HIGH;
bool vermelhoAnterior = HIGH;
bool pretoAnterior = HIGH;

int posicaoScroll = 0;
int intervaloScroll = 300;
int delayInicial = 1000;
int delayFinal = 5000;
int caracteresporScroll = 1;
int volumeAnterior = 0;
int leituraPotAnterior = 0;
bool scrollCompleto = false;
bool mensagemMostrada = false;
unsigned long tempoFinal = 0;

void setup() {
  Serial.begin(9600);
  softserial.begin(9600);
  pinMode(pinoAmarelo, INPUT_PULLUP);
  pinMode(pinoVermelho, INPUT_PULLUP);
  pinMode(pinoPreto, INPUT_PULLUP);
  pinMode(pinoLED, OUTPUT);
  pinMode(pinoBusy, INPUT);

  lcd.init();
  lcd.backlight();

  if (!dfplayer.begin(softserial)) {
    Serial.println("Erro ao inicializar dfplayer");
    lcd.setCursor(0, 0);
    lcd.print("Erro DFPlayer");
    while(true);
  }
  else {
    Serial.println("dfplayer inicializado");
  }

  int arquivosNoSD = dfplayer.readFileCounts();
  if (arquivosNoSD > 0) qtd_sons = arquivosNoSD;

  leituraPotAnterior = analogRead(pinoVolume);
  volumeAnterior = map(leituraPotAnterior, 0, 1023, 0, 30);
  dfplayer.volume(volumeAnterior);
  dfplayer.sleep();

  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
  delay(2000);
  lcd.clear();
  lcd.noBacklight();
}

void loop() {
  verificarSerial();
  verificarBotoes();
  exibirMensagem();
}

// Retorna true apenas no instante em que o botao passa de solto para apertado
bool botaoApertado(int pino, bool &estadoAnterior) {
  bool estado = digitalRead(pino);
  bool apertou = (estado == LOW && estadoAnterior == HIGH);
  estadoAnterior = estado;
  return apertou;
}

void verificarBotoes() {
  if (millis() - tempoBotao > 50) {
    tempoBotao = millis();

    // so reage se o potenciometro mexer de verdade, para o ruido do ADC nao ficar trocando o volume
    int potenciometro = analogRead(pinoVolume);
    if (abs(potenciometro - leituraPotAnterior) > 20) {
      leituraPotAnterior = potenciometro;
      int volume = map(potenciometro, 0, 1023, 0, 30);
      if (volume != volumeAnterior) {
        dfplayer.volume(volume);
        if (!browseStatus) {
          mensagem = "volume: " + String(volume);
          resetarScroll();
        }
        volumeAnterior = volume;
      }
    }

    if (botaoApertado(pinoAmarelo, amareloAnterior)) {
      arquivoSelecionado++;
      if (arquivoSelecionado > qtd_sons) arquivoSelecionado = qtd_sons;
      mensagem = "SFX: " + String(arquivoSelecionado);
      browseStatus = true;
      resetarScroll();
    }

    if (botaoApertado(pinoVermelho, vermelhoAnterior)) {
      arquivoSelecionado--;
      if (arquivoSelecionado < 1) arquivoSelecionado = 1;
      mensagem = "SFX: " + String(arquivoSelecionado);
      browseStatus = true;
      resetarScroll();
    }

    if (botaoApertado(pinoPreto, pretoAnterior)) {
      if (browseStatus) {
        tocarSom(arquivoSelecionado);
        esperarFimDoSom();
      }
      browseStatus = false;
    }
  }
}

void verificarSerial() {
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem.trim();

    tocarSom(somNotificacao);
    for (int i = 0; i < 5; i++) {
      digitalWrite(pinoLED, HIGH);
      delay(100);
      digitalWrite(pinoLED, LOW);
      delay(100);
    }
    esperarFimDoSom();
    resetarScroll();
  }
}

// Acorda o DFPlayer do sleep, toca o arquivo e espera o BUSY indicar que comecou
void tocarSom(int arquivo) {
  dfplayer.outputDevice(DFPLAYER_DEVICE_SD); // e isso que tira o modulo do sleep (start() nao acorda)
  delay(200);
  dfplayer.volume(volumeAnterior);
  dfplayer.play(arquivo);

  unsigned long inicio = millis();
  while (digitalRead(pinoBusy) == HIGH && millis() - inicio < 1000) {}
}

void esperarFimDoSom() {
  while (digitalRead(pinoBusy) == LOW) {}
  dfplayer.sleep();
}

void exibirMensagem() {
  if (mensagem.length() == 0) return;
  int tamanhoMensagem = mensagem.length();

  if (tamanhoMensagem <= 16) {
    if (!mensagemMostrada) {
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print(mensagem);
      limparRestoDaLinha(tamanhoMensagem);
      scrollCompleto = true;
      tempoFinal = millis();
      mensagemMostrada = true;
    }
  }
  else {
    if (!mensagemMostrada) {
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print(mensagem.substring(0, 16));
      mensagemMostrada = true;
      posicaoScroll = caracteresporScroll;
      tempoAnterior = millis();
      return;
    }

    if (!scrollCompleto && millis() - tempoAnterior < delayInicial && posicaoScroll == caracteresporScroll) return;

    if (!scrollCompleto && millis() - tempoAnterior >= intervaloScroll) {
      tempoAnterior = millis();
      int posicaoFinal = tamanhoMensagem - 16;
      if (posicaoScroll > posicaoFinal) posicaoScroll = posicaoFinal;

      String janela = mensagem.substring(posicaoScroll, posicaoScroll + 16);
      lcd.setCursor(0, 0);
      lcd.print(janela);

      if (posicaoScroll >= posicaoFinal) {
        scrollCompleto = true;
        tempoFinal = millis();
      } else {
        posicaoScroll += caracteresporScroll;
      }
    }
  }


  if (scrollCompleto && millis() - tempoFinal >= delayFinal && !browseStatus) {
    lcd.clear();
    lcd.noBacklight();
    mensagem = "";
    resetarScroll();
  }
}

void resetarScroll() {
  posicaoScroll = 0;
  scrollCompleto = false;
  mensagemMostrada = false;
  lcd.clear();
}

void limparRestoDaLinha(int inicio) {
  for (int i = inicio; i < 16; i++) {
    lcd.print(" ");
  }
}
