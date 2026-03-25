#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
SoftwareSerial softserial(11,12);
DFRobotDFPlayerMini dfplayer;

String mensagem = "";
int qtd_sons = 1;

const int pinoAmarelo = 4;
const int pinoVermelho = 2;
const int pinoPreto = 6;
const int pinoLED = 8;
const int pinoVolume = A0;

unsigned long tempoAnterior = 0;
unsigned long tempoBotao = 0;
unsigned long tempoVolume = 0;

int arquivoSelecionado = 1;
bool browseStatus = false;
bool blockBotao = false;

int posicaoScroll = 0;
int intervaloScroll = 300; 
int delayInicial = 1000; 
int delayFinal = 5000; 
int caracteresporScroll = 1;
int volumeAnterior = 0; 
bool scrollCompleto = false;
bool mensagemMostrada = false;
unsigned long tempoFinal = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinoAmarelo, INPUT_PULLUP);
  pinMode(pinoVermelho, INPUT_PULLUP);
  pinMode(pinoLED, OUTPUT);
  if (!dfplayer.begin(softserial)) {
    Serial.println("Erro ao inicializar dfplayer");
    while(true);
  }
  else {
    Serial.println("dfplayer inicializado");
  }
  dfplayer.volume(15);
  dfplayer.sleep();
  lcd.init();           
  lcd.backlight();

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

void verificarBotoes() {
  if (millis() - tempoBotao > 200) {
    tempoBotao = millis();

    int potenciometro = analogRead(pinoVolume);
    int volume = map(potenciometro, 0, 1023, 0, 30);
    if (volume != volumeAnterior) {
      dfplayer.volume(volume);
      if (!browseStatus) {
      mensagem = "volume: " + String(volume);
      resetarScroll();
      }
      volumeAnterior = volume;
    }

    if (digitalRead(pinoAmarelo) == LOW && !blockBotao) {
      blockBotao = true;
      arquivoSelecionado++;
      mensagem = "SFX: " + String(arquivoSelecionado);
      browseStatus = true;
      resetarScroll();
    }
    else if (digitalRead(pinoAmarelo) == HIGH) {
      blockBotao = false;
    }

    if (digitalRead(pinoVermelho) == LOW && !blockBotao) {
      blockBotao = true;
      arquivoSelecionado--;
      mensagem = "SFX: " + String(arquivoSelecionado);
      browseStatus = true;
      resetarScroll();
    }
    else if (digitalRead(pinoVermelho) == HIGH) {
      blockBotao = false;
    }

    if (digitalRead(pinoPreto) == LOW && !blockBotao) {
      if (browseStatus) {
        dfplayer.start();
        dfplayer.play(arquivoSelecionado);
      }
      browseStatus = false;
      resetarScroll();
    }
    else if (digitalRead(pinoPreto) == HIGH) {
      blockBotao = false;
    }

  }
}

void verificarSerial() {
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem.trim();

    dfplayer.start();
    dfplayer.play(8);
    for (int i = 0; i < 5; i++) {
      digitalWrite(pinoLED, HIGH);
      delay(100);
      digitalWrite(pinoLED, LOW);
      delay(100);
    }
    resetarScroll();
  }
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
      tempoAnterior = millis();
      return;
    }
    
    if (millis() - tempoAnterior < delayInicial) return;
    
    if (!scrollCompleto && millis() - tempoAnterior >= intervaloScroll) {
      tempoAnterior = millis();
      int posicaoFinal = tamanhoMensagem - 16;

      String janela = mensagem.substring(posicaoScroll, posicaoScroll + 16);
      lcd.setCursor(0, 0);
      lcd.print(janela);
  
      if (posicaoScroll >= posicaoFinal) {
        scrollCompleto = true;
        tempoFinal = millis();
      } else {
        posicaoScroll += caracteresporScroll;
        if (posicaoScroll > posicaoFinal) posicaoScroll = posicaoFinal;
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