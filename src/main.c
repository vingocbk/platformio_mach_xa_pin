#include <Arduino.h>
#include <math.h>

#define MIN_VOLTAGE_1S            	2.7     //V
#define MIN_VOLTAGE_2S				MIN_VOLTAGE_1S*2		//V
#define MIN_VOLTAGE_3S				MIN_VOLTAGE_1S*3		//V
#define MIN_VOLTAGE_4S				MIN_VOLTAGE_1S*4		//V
#define MIN_VOLTAGE_5S				MIN_VOLTAGE_1S*5		//V
#define MIN_VOLTAGE_6S				MIN_VOLTAGE_1S*6		//V
#define MAX_VOLTAGE_1S            	4.4     //V
#define MAX_VOLTAGE_2S				MAX_VOLTAGE_1S*2		//V
#define MAX_VOLTAGE_3S				MAX_VOLTAGE_1S*3		//V
#define MAX_VOLTAGE_4S				MAX_VOLTAGE_1S*4		//V
#define MAX_VOLTAGE_5S				MAX_VOLTAGE_1S*5		//V
#define MAX_VOLTAGE_6S				MAX_VOLTAGE_1S*6		//V
#define VOLTAGE_DISCHARGER_1S		3.85	//V
#define VOLTAGE_DISCHARGER_2S		VOLTAGE_DISCHARGER_1S*2		//V
#define VOLTAGE_DISCHARGER_3S		VOLTAGE_DISCHARGER_1S*3		//V
#define VOLTAGE_DISCHARGER_4S		VOLTAGE_DISCHARGER_1S*4		//V
#define VOLTAGE_DISCHARGER_5S		VOLTAGE_DISCHARGER_1S*5		//V
#define VOLTAGE_DISCHARGER_6S		VOLTAGE_DISCHARGER_1S*6		//V

#define PIN_LED_BLUE		PC4
#define PIN_LED_RED			PC5
#define PIN_LED_YELLOW		PC6
#define PIN_MOSFET			PC7

#define PIN_READ_VOLTAGE		PD2
#define PIN_READ_TEMPERATURE	PD3

#define ADC_BIT_RESOLUTION		10
/*    
Yellow    	Red     	Blue      	type
  x        	x       	o        	3.7V	(1S)
  x        	o       	x        	7.4V	(2S)
  x        	o       	o       	11.1V	(3S)
  o        	x       	x       	14.8V	(4S)
  o        	x       	o       	18.5V	(5S)
  o        	o       	x       	22.2V	(6S)
*/


float Voltage;

float Thermistor(int Vo) {
	float T,logRt,Tf,Tc;
	float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
	float Rt;
	Rt = 10000.0*((1024.0/Vo-1));
	logRt = logf(Rt);
	T = (1.0 / (A + B*logRt + C*logRt*logRt*logRt));  // We get the temperature value in Kelvin from this Stein-Hart equation
	Tc = T - 273.15;                     // Convert Kelvin to Celcius
	Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
	return Tc;
}

void showLedBattery1S(){
	digitalWrite(PIN_LED_YELLOW, LOW);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_BLUE, HIGH);
}
void showLedBattery2S(){
	digitalWrite(PIN_LED_YELLOW, LOW);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_LED_BLUE, LOW);
}
void showLedBattery3S(){
	digitalWrite(PIN_LED_YELLOW, LOW);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_LED_BLUE, HIGH);
}
void showLedBattery4S(){
	digitalWrite(PIN_LED_YELLOW, HIGH);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_BLUE, LOW);
}
void showLedBattery5S(){
	digitalWrite(PIN_LED_YELLOW, HIGH);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_BLUE, HIGH);
}
void showLedBattery6S(){
	digitalWrite(PIN_LED_YELLOW, HIGH);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_LED_BLUE, LOW);
}
void turnOffAllLed(){
	digitalWrite(PIN_LED_YELLOW, LOW);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_BLUE, LOW);
}
void turnOnAllLed(){
	digitalWrite(PIN_LED_YELLOW, HIGH);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_LED_BLUE, HIGH);
}
void setup() {
	// put your setup code here, to run once:
	Serial_begin(115200);
	pinMode(PIN_LED_BLUE, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_LED_YELLOW, OUTPUT);
	pinMode(PIN_MOSFET, OUTPUT);
	digitalWrite(PIN_MOSFET, HIGH);
	pinMode(PIN_READ_VOLTAGE, INPUT);
	delay(100);
	// ADC1_AWDChannelConfig(INTERNAL);
	int analogVoltageValue = 0;
	Voltage = 0;
	for(int i = 0; i < 10; i++){
		analogVoltageValue += analogRead(PIN_READ_VOLTAGE);
		delay(50);
		Serial_println_i(analogRead(PIN_READ_VOLTAGE));
	}
	Voltage = (float)analogVoltageValue/10.0;
	Serial_print_s("Voltatge analog: ");
	Serial_println_f(Voltage);
	if(ADC_BIT_RESOLUTION == 10){
		Voltage = Voltage*3.3/1023;
	}
	else if(ADC_BIT_RESOLUTION == 8){
		Voltage = Voltage*3.3/256;
	}
	// Voltage = map(Voltage,0,1023,0,3300)/1000.0;
	Serial_print_s("Voltatge V1: ");
	Serial_println_f(Voltage);
	Voltage = 11*Voltage + 0.7;
	Serial_print_s("Voltatge V2: ");
	Serial_println_f(Voltage);
	if(Voltage >= MIN_VOLTAGE_1S && Voltage <= MAX_VOLTAGE_1S){
		if(Voltage >= VOLTAGE_DISCHARGER_1S){
			showLedBattery1S();
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery1S();
				delay(200);
				turnOffAllLed();
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_2S && Voltage <= MAX_VOLTAGE_2S){
		if(Voltage >= VOLTAGE_DISCHARGER_2S){
			showLedBattery2S();
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery2S();
				delay(200);
				turnOffAllLed();
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_3S && Voltage <= MAX_VOLTAGE_3S){
		if(Voltage >= VOLTAGE_DISCHARGER_3S){
			showLedBattery3S();
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery3S();
				delay(200);
				turnOffAllLed();
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_4S && Voltage <= MAX_VOLTAGE_4S){
		if(Voltage >= VOLTAGE_DISCHARGER_4S){
			showLedBattery4S();
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery4S();
				delay(200);
				turnOffAllLed();
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_5S && Voltage <= MAX_VOLTAGE_5S){
		if(Voltage >= VOLTAGE_DISCHARGER_5S){
			showLedBattery5S();
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery5S();
				delay(200);
				turnOffAllLed();
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_6S && Voltage <= MAX_VOLTAGE_6S){
		if(Voltage >= VOLTAGE_DISCHARGER_6S){
			showLedBattery6S();
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery6S();
				delay(200);
				turnOffAllLed();
				delay(200);
			}
		}
	}
	else{
		for(int i = 0; i < 5; i++){
			turnOnAllLed();
			delay(200);
			turnOffAllLed();
			delay(200);
		}
	}
}

void loop() {
	// put your main code here, to run repeatedly:
	// Serial_print_s("ngoc\n");
	// Serial_println_s("ngoc1");
	// // digitalWrite(PC4, HIGH);
	// delay(1000);
	// Serial_print_s("ngoc\n");
	// Serial_println_s("ngoc1");
	// // digitalWrite(PC4, LOW);
	delay(1000);
	Serial_println_f(Thermistor(analogRead(PIN_READ_TEMPERATURE)));


}


