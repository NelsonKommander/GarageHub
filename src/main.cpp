#include <Arduino.h>
#include <WiFi.h>
#include <stdint.h>
#include <string.h>

// PINOUT
uint8_t sensPin = 4;
uint8_t redPin = 16;
uint8_t greenPin = 17;
uint8_t bluePin = 5;
uint8_t trigger = 18;
uint8_t echo = 19;

// PWM var
const int freq = 490;
const uint8_t resolution = 8;
const uint8_t redChannel = 0;
const uint8_t blueChannel = 1;
const uint8_t greenChannel = 2;

// Program vars
bool auxLight = false;
uint32_t printTimer = 0;
double distancia, distAlvo;

// Configuração do wifi
const char* ssid = "Nelson";
const char* password = "Kommander030500";
WiFiServer server(80);

// Pagina web a ser adicionada
const char* html = "<html><head> <title>GarageHub</title> <meta charset=\"UTF-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"icon\" href=\"data:,\"> <style> html { font-family: Helvetica; display: inline-block; justify-content: center; justify-items: center; background-color: #2C2F33; margin: 0px auto; text-align: center; } h1 { color: white; background-color: #23272A; padding-top: 1.2vh; padding-bottom: 1.2vh; font-size: 1.5rem; border: none; position: absolute; top: 0; left: 0; width: 100%; } h2 { color: white; padding-top: 1vh; padding-bottom: 0.3vh; padding-left: 2vw; padding-right: 2vw; font-size: 1.5rem; } p { font-size: 1rem; color: white; } div { background-color: #23272A; margin-top: 30vh; padding-left: 9px; padding-right: 9px; padding-bottom: 1vh; border: none; border-radius: 30px; display: inline-block; justify-content: center; justify-self: center; } .indicador { color: #7286DA; font-size: 1.2rem; } .on { display: inline-block; background-color: #7286DA; border: none; border-radius: 10px; color: white; padding: 0.57vh 0.6vw; text-decoration: none; font-size: 0.8rem; margin: 2px; cursor: pointer; } .off { display: inline-block; background-color: white; border: none; border-radius: 8px; color: #2C2F22; padding: 0.57vh 0.6vw; text-decoration: none; font-size: 0.8rem; cursor: pointer; } .popup { margin-top: 5vh; visibility: hidden; } </style></head><body> <h1>GarageHub</h1> <div> <h2>Distância Alvo:</h2> <p> <l id=\"distAlvo\" class=\"indicador\">0.0cm</l> <button onclick=\"editPopupShow()\" class=\"off\">Editar</button> </p> </div> <div> <h2>Distância Atual:</h2> <p> <l id=\"distancia\" class=\"indicador\">0.0cm</l> </p> </div> <div> <h2>Estado do portão:</h2> <p> <l id=\"statPortao\" id=\"portao\" class=\"indicador\">Fechado</l> </p> </div> <p><button onclick=\"toggleAux()\" class=\"off\">Luz auxiliar</button></p> <p id=\"myPopup\" class=\"popup\">Novo Alvo: <input id=\"inputTarget\" type=\"text\"><br> <button onclick=\"submit()\" class=\"on\">Ok</button> <button onclick=\"editPopupHide()\" class=\"off\">Cancelar</button> </p> <script> function SubscribePane(url) { function showMessage(message) { let alvoElem = document.getElementById(\"distAlvo\"); let distanciaElem = document.getElementById(\"distancia\"); let portaoElem = document.getElementById(\"statPortao\"); distanciaElem.innerHTML = message[\"distancia\"] + \"cm\"; portaoElem.innerHTML = message[\"portao\"] == 0 ? \"Aberto\" : \"Fechado\"; alvoElem.innerHTML = message[\"alvo\"] + \"cm\"; } async function subscribe() { let response = await fetch(url); if (response.status == 502) { await subscribe(); } else if (response.status != 200) { showMessage(response.statusText); await new Promise(resolve => setTimeout(resolve, 1000)); await subscribe(); } else { let dirtyMessage = await response.text(); let message = JSON.parse(dirtyMessage); showMessage(message); await subscribe(); } } subscribe(); } function editPopupShow() { let popup = document.getElementById(\"myPopup\"); popup.style.visibility = \"visible\"; } function editPopupHide() { let popup = document.getElementById(\"myPopup\"); popup.style.visibility = \"hidden\"; } function submit() { let input = document.getElementById(\"inputTarget\"); let inputData = input.value; console.log(inputData); var http = new XMLHttpRequest();http.open(\"POST\", \"/newtarget\", true);http.setRequestHeader(\"Content-type\", \"text/plain\");http.send(inputData);http.onload = function () {let messageElem = document.getElementById(\"distAlvo\");let message = http.responseText;messageElem.innerHTML = message;}; let popup = document.getElementById(\"myPopup\"); popup.style.visibility = \"hidden\"; } function toggleAux(){ var http = new XMLHttpRequest(); http.open(\"GET\", \"/toggleAux\"); http.send(); } new SubscribePane('/?update'); </script></body></html>";

void fadeFromTo(int from, int to){
	for(int i = 0; i < 255; i++){
		ledcWrite(to, i);
		delay(2);
		ledcWrite(from, 255 - i);
		delay(2);
	}
}

void setRGB(int r, int g, int b){
	ledcWrite(redChannel, r);
	ledcWrite(greenChannel, g);
	ledcWrite(blueChannel, b);
}

double calcDist(){
	uint32_t echoTime;
	double distance;
	// Passando o sinal do gatilho
	digitalWrite(trigger, HIGH);
	delayMicroseconds(11);
	digitalWrite(trigger, LOW);

	echoTime = pulseIn(echo, HIGH);
	distance = 0.01715*echoTime;
	return distance;
}

void lerOCorpo(WiFiClient client){
	String currentLine, lastLine;
	while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
			if(currentLine.length() == 0){
				distAlvo = lastLine.toDouble();
			} else {
				currentLine = "";
			}
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
	  } else {
		  distAlvo = currentLine.toDouble();
		  break;
	  }
	  lastLine = currentLine;
	} 
}

void setup(){
	// Inicializando sensor
	pinMode(sensPin, INPUT);
	// Inicializando os PWM
	ledcAttachPin(redPin, redChannel);
	ledcAttachPin(bluePin, blueChannel);
	ledcAttachPin(greenPin, greenChannel);
	ledcSetup(redChannel, freq, resolution);
	ledcSetup(blueChannel, freq, resolution);
	ledcSetup(greenChannel, freq, resolution);
	// Setando o gatilho e o eco do sensor como saída e entrada respectivamente
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
	// Garantindo que o gatilho está baixo antes do incio
	digitalWrite(trigger, LOW);
	// Setando distancia alvo para o padrao (5cm)
	distAlvo = 5.0;

	// Abrindo comunicação serial com baudrate 115200
	Serial.begin(115200);
	Serial.println("Setup feito com sucesso!");

	// Connect to Wi-Fi network with SSID and password
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
	  delay(500);
	  Serial.print(".");
	}
	// Print local IP address and start web server
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
}

void loop(){
	WiFiClient client = server.available();
	// Existe um cliente, hora de atualizar os dados
	if (client) {                             // if you get a client,
    Serial.println("Cliente conectado!");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          currentLine = "";
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Checando se existe algum request e o tratando
		if (currentLine.endsWith("GET / HTTP/1.1")){
			client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print(html);
            client.println();
            break;
		}
		if (currentLine.endsWith("POST /newtarget")){
			lerOCorpo(client);
			break;
		}
		if (currentLine.endsWith("GET /toggleAux")){
			auxLight = !auxLight;
			break;
		}
		if (currentLine.endsWith("GET /?update")){
			client.printf("{\"alvo\" : \"%0.2f\", \"distancia\" : \"%0.2f\", \"portao\" : \"%d\"}", distAlvo, distancia, digitalRead(sensPin));
			break;
		}
      }
    }
    // Fechando a conexão
    client.stop();
    Serial.println("Cliente desconectado!");
  }
	// Executa sem necessariamente ter um cliente
	if (auxLight){
		setRGB(255, 255, 255);
	} else if (!digitalRead(sensPin)){	
		distancia = calcDist();
		if (distancia > distAlvo*3){
			setRGB(0, 255, 0);
		} else if (distancia <= distAlvo*3 && distancia > distAlvo+1){
			setRGB(129, 126, 0);
		} else {
			setRGB(255, 0, 0);
		}
		//Serial.print(distancia);
		//Serial.println(" cm");
	} else {
		setRGB(0, 0, 0);
	}
}

