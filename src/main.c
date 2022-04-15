#include <Arduino.h>
// #include <math.h>

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

#define MAX_TEMPERATURE_TO_RESET 	80
#define MIN_TEMPERATURE_TO_START 	30
#define CALIB_TEMPERATURE 			50

// #define MODE_DEBUG
/*    
Yellow    	Red     	Blue      	type
  x        	x       	o        	3.7V	(1S)
  x        	o       	x        	7.4V	(2S)
  x        	o       	o       	11.1V	(3S)
  o        	x       	x       	14.8V	(4S)
  o        	x       	o       	18.5V	(5S)
  o        	o       	x       	22.2V	(6S)
*/

enum MODE_BATTERY{
	S1_ON,
	S2_ON,
	S3_ON,
	S4_ON,
	S5_ON,
	S6_ON,
	ALL_OFF,
	ALL_ON,
	ERROR_BAT
};
enum MODE_BATTERY mode_battery;


float Thermistor(int Vo) {
	float T,logRt,Tf,Tc;
	float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
	float Rt;
	Rt = 10000.0*((1024.0/Vo-1));
	logRt = logf(Rt);
	T = (1.0 / (A + B*logRt + C*logRt*logRt*logRt));  // We get the temperature value in Kelvin from this Stein-Hart equation
	Tc = T - 273.15 + CALIB_TEMPERATURE;                     // Convert Kelvin to Celcius
	Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
	return Tc;
}

#ifndef MODE_DEBUG
void showLedBattery(int number){
	switch (number)
	{
	case S1_ON:
		digitalWrite(PIN_LED_YELLOW, LOW);
		digitalWrite(PIN_LED_RED, LOW);
		digitalWrite(PIN_LED_BLUE, HIGH);
		break;
	case S2_ON:
		digitalWrite(PIN_LED_YELLOW, LOW);
		digitalWrite(PIN_LED_RED, HIGH);
		digitalWrite(PIN_LED_BLUE, LOW);
		break;
	case S3_ON:
		digitalWrite(PIN_LED_YELLOW, LOW);
		digitalWrite(PIN_LED_RED, HIGH);
		digitalWrite(PIN_LED_BLUE, HIGH);
		break;
	case S4_ON:
		digitalWrite(PIN_LED_YELLOW, HIGH);
		digitalWrite(PIN_LED_RED, LOW);
		digitalWrite(PIN_LED_BLUE, LOW);
		break;
	case S5_ON:
		digitalWrite(PIN_LED_YELLOW, HIGH);
		digitalWrite(PIN_LED_RED, LOW);
		digitalWrite(PIN_LED_BLUE, HIGH);
		break;
	case S6_ON:
		digitalWrite(PIN_LED_YELLOW, HIGH);
		digitalWrite(PIN_LED_RED, HIGH);
		digitalWrite(PIN_LED_BLUE, LOW);
		break;
	case ALL_OFF:
		digitalWrite(PIN_LED_YELLOW, LOW);
		digitalWrite(PIN_LED_RED, LOW);
		digitalWrite(PIN_LED_BLUE, LOW);
		break;
	case ALL_ON:
		digitalWrite(PIN_LED_YELLOW, HIGH);
		digitalWrite(PIN_LED_RED, HIGH);
		digitalWrite(PIN_LED_BLUE, HIGH);
		break;
	default:
		break;
	}
}
#endif

float readVoltage(){
	int analogVoltageValue = 0;
	for(int i = 0; i < 5; i++){
		analogVoltageValue += analogRead(PIN_READ_VOLTAGE);
		delay(50);
	}
	float Voltage = 0;
	Voltage = (float)analogVoltageValue/5;
	Voltage = Voltage*3.3/1024;
	Voltage = 11*Voltage;
	return Voltage;
}

float readTemperature(){
	float temperature = 0;
	for(int i = 0; i < 5; i++){
		temperature += Thermistor(analogRead(PIN_READ_TEMPERATURE));
		delay(50);
	}
	temperature = temperature/5.0;
	return temperature;
}

void setup() {
	// put your setup code here, to run once:
#ifdef MODE_DEBUG
	Serial_begin(115200);
#endif
	pinMode(PIN_LED_BLUE, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_LED_YELLOW, OUTPUT);
	pinMode(PIN_MOSFET, OUTPUT);
	digitalWrite(PIN_MOSFET, LOW);
	pinMode(PIN_READ_VOLTAGE, INPUT);
	pinMode(PIN_READ_TEMPERATURE, INPUT);
	delay(100);

#ifndef MODE_DEBUG
	float Voltage = readVoltage();
	if(Voltage >= MIN_VOLTAGE_1S && Voltage <= MAX_VOLTAGE_1S){
		mode_battery = S1_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_1S){
			showLedBattery(S1_ON);
			digitalWrite(PIN_MOSFET, HIGH);
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery(S1_ON);
				delay(200);
				showLedBattery(ALL_OFF);
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_2S && Voltage <= MAX_VOLTAGE_2S){
		mode_battery = S2_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_2S){
			showLedBattery(S2_ON);
			digitalWrite(PIN_MOSFET, HIGH);
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery(S2_ON);
				delay(200);
				showLedBattery(ALL_OFF);
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_3S && Voltage <= MAX_VOLTAGE_3S){
		mode_battery = S3_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_3S){
			showLedBattery(S3_ON);
			digitalWrite(PIN_MOSFET, HIGH);
		}
		else{ 
			
			for(int i = 0; i < 5; i++){
				showLedBattery(S3_ON);
				delay(200);
				showLedBattery(ALL_OFF);
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_4S && Voltage <= MAX_VOLTAGE_4S){
		mode_battery = S4_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_4S){
			showLedBattery(S4_ON);
			digitalWrite(PIN_MOSFET, HIGH);
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery(S4_ON);
				delay(200);
				showLedBattery(ALL_OFF);
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_5S && Voltage <= MAX_VOLTAGE_5S){
		mode_battery = S5_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_5S){
			showLedBattery(S5_ON);
			digitalWrite(PIN_MOSFET, HIGH);
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery(S5_ON);
				delay(200);
				showLedBattery(ALL_OFF);
				delay(200);
			}
		}
	}
	else if(Voltage >= MIN_VOLTAGE_6S && Voltage <= MAX_VOLTAGE_6S){
		mode_battery = S6_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_6S){
			showLedBattery(S6_ON);
			digitalWrite(PIN_MOSFET, HIGH);
		}
		else{
			for(int i = 0; i < 5; i++){
				showLedBattery(S6_ON);
				delay(200);
				showLedBattery(ALL_OFF);
				delay(200);
			}
		}
	}
	else{
		mode_battery = ERROR_BAT;
		for(int i = 0; i < 5; i++){
			showLedBattery(ALL_ON);
			delay(200);
			showLedBattery(ALL_OFF);
			delay(200);
		}
	}
#endif
}

void loop() {
	// put your main code here, to run repeatedly:
	float temperature = readTemperature();
	float Voltage = readVoltage();
#ifdef MODE_DEBUG
	Serial_println_f(Voltage);
	// _sleep();
#endif

#ifndef MODE_DEBUG
	switch (mode_battery)
	{
	case S1_ON:
		/* code */
		if(Voltage < VOLTAGE_DISCHARGER_1S){
			showLedBattery(ALL_OFF);	
			digitalWrite(PIN_MOSFET, LOW);
			// _sleep();
			// sleep
		}
		break;
	case S2_ON:
		/* code */
		if(Voltage < VOLTAGE_DISCHARGER_2S){
			showLedBattery(ALL_OFF);	
			digitalWrite(PIN_MOSFET, LOW);
		}
		break;
	case S3_ON:
		/* code */
		if(Voltage < VOLTAGE_DISCHARGER_3S){
			showLedBattery(ALL_OFF);	
			digitalWrite(PIN_MOSFET, LOW);
		}
		break;
	case S4_ON:
		/* code */
		if(Voltage < VOLTAGE_DISCHARGER_4S){
			showLedBattery(ALL_OFF);	
			digitalWrite(PIN_MOSFET, LOW);
		}
		break;
	case S5_ON:
		/* code */
		if(Voltage < VOLTAGE_DISCHARGER_5S){
			showLedBattery(ALL_OFF);	
			digitalWrite(PIN_MOSFET, LOW);
		}
		break;
	case S6_ON:
		/* code */
		if(Voltage < VOLTAGE_DISCHARGER_6S){
			showLedBattery(ALL_OFF);	
			digitalWrite(PIN_MOSFET, LOW);
		}
		break;
	default:
		break;
	}

	if(temperature > MAX_TEMPERATURE_TO_RESET){
		showLedBattery(ALL_OFF);	
		digitalWrite(PIN_MOSFET, LOW);
	}
#endif

	delay(1000);
}




