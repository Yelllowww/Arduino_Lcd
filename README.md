![image](https://github.com/Yelllowww/Arduino_Lcd/blob/main/circuit_scheme.png)
## Projeto sendo desenvolvido com:

* Arduino nano (não original)
* Tela lcd com módulo I2C
* Impressão 3d
* Módulo de áudio DFPlayer
* Módulo de amplificador
* Auto-falante
* Botões avulsos

## Funcionalidade:
O input da string via porta serial chega, a backlight liga, e aparece na tela. Se o texto for maior que o limite, há um delay que mostra o começo da mensagem e em seguida começa a rolagem do texto.
Quando a mensagem chega ao fim, ocorre mais um delay, a tela fica limpa e a backlight é desligada. Dentro do loop de execução, ele verifica o input da porta serial, verifica o input da porta dos botões, e chama a função de exibição até que a mensagem seja atribuída.
A função de verificar os botões esperará o sinal de três botões que funcionarão como dois seletores e um de confirmação, onde selecionarão o arquivo de efeito sonoro dentro do cartão do DFPlayer e o outro confirmará o arquivo. A cada aperto de botão, seria demonstrado
na tela cada opção, como um menu.


## Possíveis implementações futuras (só possíveis mesmo):
* Indicador de LED
* LED rgb com input de troca de cor
* Transmissão por módulo bluetooth/wireless
* Comandos por reconhecimento de voz
* Comandos por palma (volume spike)

## Ler para executar o próximo passo:
### Circuito para o Arduino UNO
DFPlayer Mini (pino 1 no canto superior esquerdo, com o slot do SD para cima):

| Pino DFPlayer | Liga em |
|---|---|
| 1 VCC | 5V |
| 2 RX | Resistor de 1 kΩ → D12 |
| 3 TX | D11 |
| 7 GND ou 10 GND | GND |
| 16 BUSY | D3 |
| 6 SPK_1 / 8 SPK_2 | Alto-falante direto (8 Ω, até 3 W) **ou** use o amplificador |
| 4 DAC_R / 5 DAC_L | Entradas R/L do amplificador (só se usar amplificador) |

Com o módulo amplificador (ex.: PAM8403): ligue DAC_L (pino 5) e DAC_R (pino 4) nas entradas L/R do amplificador. Ligue o GND do DFPlayer no GND de entrada do amplificador e o alto-falante na saída dele. Nunca ligue SPK_1 ou SPK_2 ao GND nem ao amplificador, porque essa saída é em ponte e pode queimar o módulo.

Coloque um capacitor de 100–470 µF entre VCC e GND perto do DFPlayer. Sem ele, os picos de som podem reiniciar o Arduino ou causar chiado.

LCD 16x2 com I2C: GND → GND, VCC → 5V, SDA → A4, SCL → A5.

Potenciômetro (10 kΩ): uma ponta no 5V, a outra no GND e o pino do meio no A0.

Botões: cada um tem um lado ligado ao pino e o outro no GND. Não precisa de resistor, porque o código usa o pull-up interno.

- Amarelo (próximo): D4
- Vermelho (anterior): D2
- Preto (tocar): D6
LED: D8 → resistor de 220 Ω → perna longa (+) do LED. A perna curta vai no GND.
Os pinos do seu desenho no Tinkercad batem com os do código.

### Cartão SD
Formate em FAT32.
Nomeie os arquivos 0001.mp3, 0002.mp3, … na raiz.
play(n) segue a ordem em que os arquivos foram copiados, não o nome. Formate o cartão e copie os arquivos um por um, em ordem.
O arquivo 0008 é o som de notificação das mensagens seriais. Troque somNotificacao no código se quiser outro.
### Se algo não funcionar
Se o DFPlayer não voltar do sleep (alguns clones não voltam), apague as linhas dfplayer.sleep(). O resto continua funcionando.
Se o LCD acender sem texto, mude o endereço para 0x3F ou ajuste o trimpot azul atrás do módulo I2C.
No Monitor Serial, use 9600 baud com final de linha "Nova linha" (Newline).
Evite acentos nas mensagens, porque o LCD não mostra acentos corretamente.
