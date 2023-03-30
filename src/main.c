#include <Arduino.h>
// #include <math.h>

#define MODE_DISCHARGER_3V8

#ifdef MODE_DISCHARGER_3V8
#define MIN_VOLTAGE_1S            	3.4     //V
#define VOLTAGE_DISCHARGER_1S		3.85	//V
#define MAX_VOLTAGE_1S            	4.2     //V
#else 
#define MIN_VOLTAGE_1S            	3.25    //V
#define VOLTAGE_DISCHARGER_1S		3.3		//V
#define MAX_VOLTAGE_1S            	4.0     //V
#endif

#define MIN_VOLTAGE_2S				MIN_VOLTAGE_1S*2		//V
#define MIN_VOLTAGE_3S				MIN_VOLTAGE_1S*3		//V
#define MIN_VOLTAGE_4S				MIN_VOLTAGE_1S*4		//V
#define MIN_VOLTAGE_5S				MIN_VOLTAGE_1S*5		//V
#define MIN_VOLTAGE_6S				MIN_VOLTAGE_1S*6		//V

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


// #define PIN_LED_BLUE		PC5
// #define PIN_LED_RED			PC6
// #define PIN_LED_GREEN		PC7
#define PIN_LED				PC5
#define PIN_MOSFET			PC4
// #define PIN_MOSFET			PC3
#define PIN_RELAY			PC7

#define VALUE_PWM_1S		255
#define VALUE_PWM_2S		255
#define VALUE_PWM_3S		220
#define VALUE_PWM_4S		180
#define VALUE_PWM_5S		120
#define VALUE_PWM_6S		60
#define VALUE_PWM_OFF		0
// #define VALUE_PWM_1S		0
// #define VALUE_PWM_2S		0
// #define VALUE_PWM_3S		0
// #define VALUE_PWM_4S		0
// #define VALUE_PWM_5S		0
// #define VALUE_PWM_6S		0
// #define VALUE_PWM_OFF		0

#define PIN_READ_VOLTAGE		PD2
#define PIN_READ_TEMPERATURE	PD3

#define MAX_TEMPERATURE_TO_RESET 	80
#define MIN_TEMPERATURE_TO_START 	30
#define CALIB_TEMPERATURE 			50

#define MODE_DELAY_LOW_VOLTAGE		200
#define MOD_DELAY_WRONG_VOLTAGE		500

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
void showLedBattery(int number){
	switch (number)
	{
	case S1_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	case S2_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	case S3_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	case S4_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	case S5_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	case S6_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	case ALL_OFF:
		digitalWrite(PIN_LED, LOW);
		break;
	case ALL_ON:
		digitalWrite(PIN_LED, HIGH);
		break;
	default:
		break;
	}
}

void turnOffLedAndMofet(){
	showLedBattery(ALL_OFF);	
	analogWrite(PIN_MOSFET, VALUE_PWM_OFF);
	digitalWrite(PIN_RELAY, LOW);
}

void turnOnLedAndMofet(int number){
	// analogWrite(PIN_MOSFET, VALUE_PWM_OFF);
	showLedBattery(number);
	// return;
	switch (number)
	{
	case S1_ON:
		analogWrite(PIN_MOSFET, VALUE_PWM_1S);
		break;
	case S2_ON:
		analogWrite(PIN_MOSFET, VALUE_PWM_2S);
		break;
	case S3_ON:
		analogWrite(PIN_MOSFET, VALUE_PWM_3S);
		break;
	case S4_ON:
		analogWrite(PIN_MOSFET, VALUE_PWM_4S);
		break;
	case S5_ON:
		analogWrite(PIN_MOSFET, VALUE_PWM_5S);
		break;
	case S6_ON:
		analogWrite(PIN_MOSFET, VALUE_PWM_6S);
		break;	
	default:
		break;
	}
}

void ledLowVoltage(int number, int time_delay){
	for(int i = 0; i < 5; i++){
		showLedBattery(number);
		delay(time_delay);
		showLedBattery(ALL_OFF);
		delay(time_delay);
	}
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
	delay(500);
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
	if(Voltage >= MIN_VOLTAGE_6S && Voltage <= MAX_VOLTAGE_6S){
		mode_battery = S6_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_6S){
			turnOnLedAndMofet(S6_ON);
		}
		else{
			ledLowVoltage(S6_ON);
		}
	}
	else if(Voltage >= MIN_VOLTAGE_5S && Voltage <= MAX_VOLTAGE_5S){
		mode_battery = S5_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_5S){
			turnOnLedAndMofet(S5_ON);
		}
		else{
			ledLowVoltage(S1_ON);
		}
	}
	else if(Voltage >= MIN_VOLTAGE_4S && Voltage <= MAX_VOLTAGE_4S){
		mode_battery = S4_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_4S){
			turnOnLedAndMofet(S4_ON);
		}
		else{
			ledLowVoltage(S2_ON);
		}
	}
	else if(Voltage >= MIN_VOLTAGE_3S && Voltage <= MAX_VOLTAGE_3S){
		mode_battery = S3_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_3S){
			turnOnLedAndMofet(S3_ON);
		}
		else{ 
			ledLowVoltage(S3_ON, MODE_DELAY_LOW_VOLTAGE);
		}
	}
	else if(Voltage >= MIN_VOLTAGE_2S && Voltage <= MAX_VOLTAGE_2S){
		mode_battery = S2_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_2S){
			turnOnLedAndMofet(S2_ON);
		}
		else{
			ledLowVoltage(S4_ON);
		}
	}
	else if(Voltage >= MIN_VOLTAGE_1S && Voltage <= MAX_VOLTAGE_1S){
		mode_battery = S1_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_1S){
			turnOnLedAndMofet(S1_ON);
		}
		else{
			ledLowVoltage(S5_ON);
		}
	}
	else if(Voltage >= MIN_VOLTAGE_6S && Voltage <= MAX_VOLTAGE_6S){
		mode_battery = S6_ON;
		if(Voltage >= VOLTAGE_DISCHARGER_6S){
			turnOnLedAndMofet(S6_ON);
		}
		else{
			ledLowVoltage(S6_ON);
		}
	}
	else{
		mode_battery = ERROR_BAT;
		ledLowVoltage(ALL_ON, MOD_DELAY_WRONG_VOLTAGE);
	}
#endif

	// while(1){
	// 	for(int i = 0; i < 255; i++){
	// 		analogWrite(PIN_LED_GREEN,i);
	// 		analogWrite(PIN_LED_RED,i);
	// 		analogWrite(PIN_LED_BLUE,i);
	// 		delay(10);
	// 	}
	// }
	
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
	// digitalWrite(PIN_LED_BLUE, LOW);
	// digitalWrite(PIN_LED_RED, LOW);
	// digitalWrite(PIN_LED_GREEN, LOW);
	// halt();
#endif

#ifndef MODE_DEBUG
	if(millis() - count_time >= 1000){
		count_time = millis();
		float temperature = readTemperature();
		float Voltage = readVoltage();
		switch (mode_battery)
		{
		case S1_ON:
			/* code */
			if(Voltage < VOLTAGE_DISCHARGER_1S){
				turnOffLedAndMofet();
			}
			break;
		case S2_ON:
			/* code */
			if(Voltage < VOLTAGE_DISCHARGER_2S){
				turnOffLedAndMofet();
			}
			break;
		case S3_ON:
			/* code */
			if(Voltage < VOLTAGE_DISCHARGER_3S){
				turnOffLedAndMofet();
			}
			break;
		case S4_ON:
			/* code */
			if(Voltage < VOLTAGE_DISCHARGER_4S){
				turnOffLedAndMofet();
			}
			break;
		case S5_ON:
			/* code */
			if(Voltage < VOLTAGE_DISCHARGER_5S){
				turnOffLedAndMofet();
			}
			break;
		case S6_ON:
			/* code */
			if(Voltage < VOLTAGE_DISCHARGER_6S){
				turnOffLedAndMofet();
			}
			break;
		default:
			break;
		}
		if(temperature > MAX_TEMPERATURE_TO_RESET){
			analogWrite(PIN_MOSFET, VALUE_PWM_OFF);
		}
		else{
			turnOnLedAndMofet(mode_battery);
		}
	}

#endif

}




