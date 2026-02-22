#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int backlightPin = 10;

String mensagem = "";
unsigned long tempoAnterior = 0;
unsigned long tempoBotao = 0;
int posicaoScroll = 0;
int intervaloScroll = 0; 
int delayInicial = 1000;
int delayFinal = 5000;
int caracteresporScroll = 4;
bool scrollCompleto = false;
bool mensagemMostrada = false;
unsigned long tempoFinal = 0;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  lcd.setCursor(0, 0);
  lcd.print("Sistema Pronto");
  delay(2000);
  lcd.clear();
  pinMode(backlightPin, OUTPUT);
  analogWrite(backlightPin, 0);
}

void loop() {
  verificarSerial();
  verificarBotoes();
  exibirMensagem();
}

void verificarSerial() {
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem.trim();
    resetarScroll();
  }
}

void verificarBotoes() {
  int valorBotao = analogRead(0);
  
  if (valorBotao < 800 && millis() - tempoBotao > 200) {
    tempoBotao = millis();
    
    if (valorBotao < 60) {
      mensagem = "1";
      resetarScroll();

    } else if (valorBotao < 200) {
      mensagem = "2";  
      resetarScroll();
      
    } else if (valorBotao < 400) {
      mensagem = "3";  
      resetarScroll();
      
    } else if (valorBotao < 600) {
      mensagem = "4"; 
      resetarScroll();
      
    } else if (valorBotao < 800) {
      mensagem = "5";  
      resetarScroll();
    }
  }
}

void exibirMensagem() {
  if (mensagem.length() == 0) return;
  int tamanhoMensagem = mensagem.length();
  
  if (tamanhoMensagem <= 16) {
    if (!mensagemMostrada) {
    lcd.setCursor(0, 0);
    analogWrite(backlightPin, 255);
    lcd.print(mensagem);
    limparRestoDaLinha(tamanhoMensagem);
    scrollCompleto = true;
    tempoFinal = millis();
    mensagemMostrada = true;
    }
  } 
  else {
    if (!mensagemMostrada) {
      lcd.setCursor(0, 0);
      analogWrite(backlightPin, 255);
      lcd.print(mensagem.substring(0, 16));
      mensagemMostrada = true;
      tempoAnterior = millis();
      return;
    }
    
    if (millis() - tempoAnterior < delayInicial) {
      return;
    }
    
    if (posicaoScroll == 0) {
      lcd.clear();
      tempoAnterior = millis();
    }
    
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
        if (posicaoScroll + caracteresporScroll > posicaoFinal) {
      posicaoScroll = posicaoFinal;
        } 
        else {
          posicaoScroll += caracteresporScroll;
        }
      }
    }
  }
  if (scrollCompleto && millis() - tempoFinal >= delayFinal) {
    lcd.clear();
    mensagem = "";
    resetarScroll();
    analogWrite(backlightPin, 0);
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