#include <Arduino.h>

#define	TYPE_BATTERY_2S
// #define	TYPE_BATTERY_3S
// #define	TYPE_BATTERY_4S
// #define	TYPE_BATTERY_6S

#define VOLTAGE_DISCHARGER_1S		3.85		//V


#define MIN_VOLTAGE_1S            	3.0     //V
#define MIN_VOLTAGE_2S				MIN_VOLTAGE_1S*2		//V
#define MIN_VOLTAGE_3S				MIN_VOLTAGE_1S*3		//V
#define MIN_VOLTAGE_4S				MIN_VOLTAGE_1S*4		//V
#define MIN_VOLTAGE_5S				MIN_VOLTAGE_1S*5		//V
#define MIN_VOLTAGE_6S				MIN_VOLTAGE_1S*6		//V
#define MAX_VOLTAGE_1S            	4.2     //V
#define MAX_VOLTAGE_2S				MAX_VOLTAGE_1S*2		//V
#define MAX_VOLTAGE_3S				MAX_VOLTAGE_1S*3		//V
#define MAX_VOLTAGE_4S				MAX_VOLTAGE_1S*4		//V
#define MAX_VOLTAGE_5S				MAX_VOLTAGE_1S*5		//V
#define MAX_VOLTAGE_6S				MAX_VOLTAGE_1S*6		//V
#define VOLTAGE_DISCHARGER_2S		VOLTAGE_DISCHARGER_1S*2		//V
#define VOLTAGE_DISCHARGER_3S		VOLTAGE_DISCHARGER_1S*3		//V
#define VOLTAGE_DISCHARGER_4S		VOLTAGE_DISCHARGER_1S*4		//V
#define VOLTAGE_DISCHARGER_5S		VOLTAGE_DISCHARGER_1S*5		//V
#define VOLTAGE_DISCHARGER_6S		VOLTAGE_DISCHARGER_1S*6		//V

#define PIN_LED						PC5
#define PIN_MOSFET					PC4
#define PIN_RELAY					PC7
#define PIN_READ_VOLTAGE			PD2
#define PIN_READ_TEMPERATURE		PD3

#define LED_ON						HIGH
#define LED_OFF						LOW

#define VALUE_PWM_1S				255
#define VALUE_PWM_2S				255
#define VALUE_PWM_3S				230
#define VALUE_PWM_4S				170
#define VALUE_PWM_5S				120
#define VALUE_PWM_6S				110
#define VALUE_PWM_OFF				0

#define MAX_TEMPERATURE_TO_RESET 	80
#define CALIB_TEMPERATURE 			50

#define MODE_DELAY_LOW_VOLTAGE		500
#define MOD_DELAY_WRONG_VOLTAGE		100

#define MODE_POWER_5V

// #define MODE_DEBUG
/*    
Green    	Red     	Blue      	type
  x        	x       	o        	3.7V	(1S)
  x        	o       	x        	7.4V	(2S)
  x        	o       	o       	11.1V	(3S)
  o        	x       	x       	14.8V	(4S)
  o        	x       	o       	18.5V	(5S)
  o        	o       	x       	22.2V	(6S)
*/

enum MODE_BATTERY mode_battery;
unsigned long count_time = 0;

float Thermistor(int Vo) {
	float T,logRt,Tf,Tc;
	float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
	float Rt;
	Rt = 100000.0*((1024.0/Vo-1));
	logRt = logf(Rt);
	T = (1.0 / (A + B*logRt + C*logRt*logRt*logRt));  // We get the temperature value in Kelvin from this Stein-Hart equation
	Tc = T - 273.15 + CALIB_TEMPERATURE;                     // Convert Kelvin to Celcius
	Tf = (Tc * 1.8) + 32.0;              // Convert Kelvin to Fahrenheit
	return Tc;
}

#ifndef MODE_DEBUG
void turnOffLedAndMofet(){
	digitalWrite(PIN_LED, LED_OFF);
	analogWrite(PIN_MOSFET, VALUE_PWM_OFF);
	digitalWrite(PIN_RELAY, LOW);
}

void turnOnLedAndMofet(){
	digitalWrite(PIN_LED, LED_ON);
#ifdef TYPE_BATTERY_2S
	analogWrite(PIN_MOSFET, VALUE_PWM_2S);
#endif	//TYPE_BATTERY_2S
#ifdef TYPE_BATTERY_3S
	analogWrite(PIN_MOSFET, VALUE_PWM_3S);
#endif	//TYPE_BATTERY_3S
#ifdef TYPE_BATTERY_4S
	analogWrite(PIN_MOSFET, VALUE_PWM_4S);
#endif	//TYPE_BATTERY_4S
#ifdef TYPE_BATTERY_6S
	analogWrite(PIN_MOSFET, VALUE_PWM_6S);
#endif	//TYPE_BATTERY_6S
}

void setLedBlinkBeforeOff(int time_delay){
	for(int i = 0; i < 5; i++){
		digitalWrite(PIN_LED, LED_ON);
		delay(time_delay);
		digitalWrite(PIN_LED, LED_OFF);
		delay(time_delay);
	}
	digitalWrite(PIN_RELAY, LOW);
}

#endif

float readVoltage(){
	int analogVoltageValue = 0;
	for(int i = 0; i < 5; i++){
		analogVoltageValue += analogRead(PIN_READ_VOLTAGE);
		delay(5);
	}
	float Voltage = 0;
	Voltage = (float)analogVoltageValue/5.0;
#ifdef MODE_POWER_5V
	Voltage = Voltage*5.0/1024.0;
#else
	Voltage = Voltage*3.2/1024.0;
#endif
	Voltage = 11.0*Voltage;
	return Voltage;
}

float readTemperature(){
	float temperature = 0;
	for(int i = 0; i < 5; i++){
		temperature += Thermistor(analogRead(PIN_READ_TEMPERATURE));
		delay(5);
	}
	temperature = temperature/5.0;
	return temperature;
}

void setup() {
	// put your setup code here, to run once:
	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_RELAY, OUTPUT);
	pinMode(PIN_READ_VOLTAGE, INPUT);
	pinMode(PIN_READ_TEMPERATURE, INPUT);
	delay(10);
	digitalWrite(PIN_RELAY, HIGH);
	analogWrite(PIN_MOSFET, VALUE_PWM_OFF);
#ifdef MODE_DEBUG
	Serial_begin(115200);
	digitalWrite(PIN_LED_BLUE, HIGH);
	digitalWrite(PIN_LED_RED, HIGH);
	digitalWrite(PIN_LED_GREEN, HIGH);
	// digitalWrite(PIN_MOSFET, HIGH);
#endif
	

#ifndef MODE_DEBUG
	float Voltage = readVoltage();
#ifdef TYPE_BATTERY_2S
	if(Voltage >= MIN_VOLTAGE_2S && Voltage <= MAX_VOLTAGE_2S){
		if(Voltage >= VOLTAGE_DISCHARGER_2S){
			turnOnLedAndMofet();
		}
		else{
			setLedBlinkBeforeOff(MODE_DELAY_LOW_VOLTAGE);
		}
	}
	else{
		setLedBlinkBeforeOff(MOD_DELAY_WRONG_VOLTAGE);
	}
#endif //TYPE_BATTERY_2S

#ifdef TYPE_BATTERY_3S
	if(Voltage >= MIN_VOLTAGE_3S && Voltage <= MAX_VOLTAGE_3S){
		if(Voltage >= VOLTAGE_DISCHARGER_3S){
			turnOnLedAndMofet();
		}
		else{
			setLedBlinkBeforeOff(MODE_DELAY_LOW_VOLTAGE);
		}
	}
	else{
		setLedBlinkBeforeOff(MOD_DELAY_WRONG_VOLTAGE);
	}
#endif //TYPE_BATTERY_3S

#ifdef TYPE_BATTERY_4S
	if(Voltage >= MIN_VOLTAGE_4S && Voltage <= MAX_VOLTAGE_4S){
		if(Voltage >= VOLTAGE_DISCHARGER_4S){
			turnOnLedAndMofet();
		}
		else{
			setLedBlinkBeforeOff(MODE_DELAY_LOW_VOLTAGE);
		}
	}
	else{
		setLedBlinkBeforeOff(MOD_DELAY_WRONG_VOLTAGE);
	}
#endif //TYPE_BATTERY_4S

#ifdef TYPE_BATTERY_6S
	if(Voltage >= MIN_VOLTAGE_6S && Voltage <= MAX_VOLTAGE_6S){
		if(Voltage >= VOLTAGE_DISCHARGER_6S){
			turnOnLedAndMofet();
		}
		else{
			setLedBlinkBeforeOff(MODE_DELAY_LOW_VOLTAGE);
		}
	}
	else{
		setLedBlinkBeforeOff(MOD_DELAY_WRONG_VOLTAGE);
	}
#endif //TYPE_BATTERY_6S
#endif	//MODE_DEBUG
}

void loop() {
	// put your main code here, to run repeatedly:
#ifdef MODE_DEBUG
	float temperature = readTemperature();
	float Voltage = readVoltage();
	Serial_println_f(Voltage);
	Serial_println_f(temperature);
	// _sleep();
	delay(1000);
	digitalWrite(PIN_LED_BLUE, LOW);
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_GREEN, LOW);
	halt();
#endif

#ifndef MODE_DEBUG
	if(millis() - count_time >= 1000){
		count_time = millis();
		float temperature = readTemperature();
		float Voltage = readVoltage();
#ifdef TYPE_BATTERY_2S
		if(Voltage < VOLTAGE_DISCHARGER_2S){
			turnOffLedAndMofet();
		}
#endif //TYPE_BATTERY_2S
#ifdef TYPE_BATTERY_3S
		if(Voltage < VOLTAGE_DISCHARGER_3S){
			turnOffLedAndMofet();
		}
#endif //TYPE_BATTERY_3S
#ifdef TYPE_BATTERY_4S
		if(Voltage < VOLTAGE_DISCHARGER_4S){
			turnOffLedAndMofet();
		}
#endif //TYPE_BATTERY_4S
#ifdef TYPE_BATTERY_6S
		if(Voltage < VOLTAGE_DISCHARGER_6S){
			turnOffLedAndMofet();
		}
#endif //TYPE_BATTERY_6S
		if(temperature > MAX_TEMPERATURE_TO_RESET){
			analogWrite(PIN_MOSFET, VALUE_PWM_OFF);
		}
		else{
			turnOnLedAndMofet();
		}
	}
#endif	//MODE_DEBUG
}




