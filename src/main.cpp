#include <Arduino.h>
#include <stdint.h>

// PINOUT
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


uint32_t printTimer = 0;
double distancia;

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

void setup(){
	// Teste da fita (ascender branca)
	/*pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
	pinMode(redPin, OUTPUT);
	digitalWrite(greenPin, 1);
	digitalWrite(bluePin, 1);
	digitalWrite(redPin, 1);*/

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
	// Abrindo comunicação serial com baudrate 115200
	Serial.begin(115200);
	Serial.println("Setup feito com sucesso!");
}

void loop(){
		
		// Codigo de exemplo alteração de cores analogica
		/*fadeFromTo(blueChannel, greenChannel);
		Serial.println("I'm faded");
		fadeFromTo(greenChannel, redChannel);
		Serial.println("Where are you now?");
		fadeFromTo(redChannel, blueChannel);
		Serial.println("Under de seaa");*/
		
		/* Codigo de exemplo calculo de distancia
		if(millis() - printTimer > 500){
			printTimer = millis();
			double distancia = 0;
			distancia = calcDist();
			Serial.print(distancia);
			Serial.println(" cm");
		}*/
		distancia = calcDist();
		if (distancia > 50){
			setRGB(0, 255, 0);
		} else if (distancia <= 50 && distancia > 20){
			setRGB(129, 126, 0);
		} else {
			setRGB(255, 0, 0);
		}
		Serial.print(distancia);
		Serial.println(" cm");
}

