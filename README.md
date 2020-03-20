# GarageHub
Sistema conectado de estacionamento com indicador de distância de parada e luz
auxiliar para serviço.

O GarageHub é composto por um controlador ESP32, um sensor de distância ultrassônico,
fita de led RGB (que funciona como indicador de distância e luz auxiliar de serviço) e
sensor fim de curso magnético.

Através destes componentes o GarageHub consegue comunicar se o portão está aberto ou fechado,
indica quando você está na distância ideal da parede e ainda serve como iluminação auxiliar para
trabalhar no seu veículo.

Você consegue saber se o portão de casa está aberto ou fechado e alterar a distância através da
aplicação web que pode ser acessada através do endereço de ip impresso na porta serial durante o boot


## TO DO:
* ~~Ajustar os limiares das cores de modo que eles tomem como base a distância alvo~~
* ~~Alterar o código para funcionar com ESP32 (atualmente rodando no arduino uno)~~
* ~~Implementar o WebServer e colocar a página html no ar~~
* ~~Adicionar suporte ao sensor de fim de curso do portão~~
* ~~Limitar as luzes do modo distância a funcionar apenas com o portão aberto~~
* ~~Adicionar modo iluminação auxiliar (luz branca)~~
