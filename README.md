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
