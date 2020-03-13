#include <Arduino.h>
#include <stdint.h>

uint8_t redPin = 11;
uint8_t greenPin = 10;
uint8_t bluePin = 9;
uint8_t trigger = 2;
uint8_t echo = 4;
uint32_t printTimer = 0;
double distancia;

void fadeFromTo(int from, int to){
	for(int i = 0; i < 255; i++){
		analogWrite(to, i);
		delay(2);
		analogWrite(from, 255 - i);
		delay(2);
	}
}

void setRGB(int r, int g, int b){
	digitalWrite(redPin, r);
	digitalWrite(greenPin, g);
	digitalWrite(bluePin, b);
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
	// Setando os canais RGB como saída
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
	pinMode(redPin, OUTPUT);
	// Setando o gatilho e o eco do sensor como saída e entrada respectivamente
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
	// Garantindo que o gatilho está baixo antes do incio
	digitalWrite(trigger, LOW);
	// Abrindo comunicação serial com baudrate 115200
	Serial.begin(115200);
	Serial.println("Iniciando as medições!");
}

void loop(){
		
		// Codigo de exemplo alteração de cores analogica
		/*fadeFromTo(bluePin, greenPin);
		fadeFromTo(greenPin, redPin);
		fadeFromTo(redPin, bluePin);*/
		
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

