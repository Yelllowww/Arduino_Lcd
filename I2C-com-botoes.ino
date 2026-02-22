#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); 

String mensagem = "";
const int pinoAmarelo = 6;
const int pinoVermelho = 10;
const int pinoLED = 8;
unsigned long tempoAnterior = 0;
unsigned long tempoBotao = 0;
int posicaoScroll = 0;
int intervaloScroll = 300; 
int delayInicial = 1000; 
int delayFinal = 5000; 
int caracteresporScroll = 1; 
bool scrollCompleto = false;
bool mensagemMostrada = false;
unsigned long tempoFinal = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinoAmarelo, INPUT_PULLUP);
  pinMode(pinoVermelho, INPUT_PULLUP);
  pinMode(pinoLED, OUTPUT);
  lcd.init();           
  lcd.backlight();     
  
  lcd.setCursor(0, 0);
  lcd.print("Sistema Pronto");
  delay(2000);
  lcd.clear();
  lcd.noBacklight();    
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
  if (millis() - tempoBotao > 200) {
    tempoBotao = millis();
    if (digitalRead(pinoAmarelo) == LOW) {
      mensagem = "LED ligado";
      digitalWrite(pinoLED, HIGH);
      resetarScroll();
    }
    else if (digitalRead(pinoVermelho) == LOW) {
      mensagem = "LED desligado";
      digitalWrite(pinoLED, LOW);
      resetarScroll();
    }
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

  
  if (scrollCompleto && millis() - tempoFinal >= delayFinal) {
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