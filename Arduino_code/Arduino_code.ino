#include "DHT.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// Pin digitale di arduino connesso al DHT
#define DHTPIN 8
// tipo del sensore: DHT 11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Comunicazione seriale con esp
SoftwareSerial nodemcu(10, 11);
char c;

//Sensore luminosità
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
#define luxPin 4

//Sensore umidità terreno
const int hygrometer = A0;
int Hterreno;

//Sensore livello acqua
#define sensorPin A1
int livelloAcqua = 0;
#define sensorPower 7

void configureSensor(void)
{
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */    
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
}

void setup() {

  //Comunicazione seriale per controllo su pc
  Serial.begin(57600);
  //Comunicazione seriale tra schede
  nodemcu.begin(9600);
  //Inizializzazione sensore umidità e temperatura
  dht.begin();
  pinMode(sensorPower, OUTPUT);
  digitalWrite(sensorPower, LOW);
  pinMode(luxPin, OUTPUT);
  if(!tsl.begin()){
    Serial.println("Errore TLS2561");
  }
  configureSensor();
}
 
void loop () {

  //Lettura humidità
  int h = dht.readHumidity();
  // Lettura della temperatura in gradi Celsius
  int t = dht.readTemperature();

  //Lettura luce
  sensors_event_t event;
  tsl.getEvent(&event);
  int luce;
  //Mostra il valore della luce in lux
  if (event.light < 200){
    luce = event.light;
    digitalWrite(luxPin, HIGH);
  }else{
    luce = event.light;
    digitalWrite(luxPin, LOW);
  }

  //Lettura umidità terreno
  Hterreno = analogRead(hygrometer);
	Hterreno = constrain(Hterreno,350,1023);	
	Hterreno = map(Hterreno,350,1023,100,0);	//ADC

  //Lettura livello acqua
  int livelloA = readSensor();
	
  String acqua = "";

  if(livelloA <= 135){
    acqua = "Empty";
  }else if(livelloA > 135 && livelloA <= 290){
    acqua = "Low";
  }else if(livelloA > 290 && livelloA <= 350){
    acqua = "Medium";
  }else if(livelloA > 350){
    acqua = "High";
  }

  String str;
  
  //Lettura richiesta e relative risposte alla shceda esp
  while(nodemcu.available() > 0){
        c = nodemcu.read();
        if(c == '1') {
          nodemcu.print("H "+String(h)+"% Temp "+String(t)+"C Soil's h "+ String(Hterreno)+"% Acqua " +acqua + " L "+String(luce)+" lumen>");
        }else if(c == '2'){
          nodemcu.print("humidity "+String(h)+"%>");
        }else if(c == '3'){
          nodemcu.print("temperature "+String(t)+"C>");
        }else if(c == '4'){
          nodemcu.print("Soil's humidity "+ String(Hterreno)+"%>");
        }else if(c == '5'){
          nodemcu.print("Acqua " +acqua + ">");
        }else if(c == '6'){
          nodemcu.print("lux "+String(luce)+" lumen>");
        }else {
          c = 0;  
          break;
        }
  }
  delay(2000);
}

int readSensor() {
  digitalWrite(sensorPower, HIGH);
	delay(100);	
	livelloAcqua = analogRead(sensorPin);
  digitalWrite(sensorPower, LOW);
	return livelloAcqua;
}