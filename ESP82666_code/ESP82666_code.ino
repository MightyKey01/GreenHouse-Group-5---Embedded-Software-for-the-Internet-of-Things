#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SoftwareSerial.h>

//Credenziali per connessione a wifi
#define WIFI_SSID "Io"
#define WIFI_PASSWORD "iosonorada"

//
#define BOT_TOKEN "5908268133:AAGp11I9XSCFF_oeyIwA4X5qgMIoPuEZlSI"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; 

//Variabili per lettura seriale
char c;

SoftwareSerial nodemcu(D1, D2);

void handleNewMessages(int numNewMessages){
  Serial.print("Messaggio ricevuto dal bot ");

  for (int i = 0; i < numNewMessages; i++){

    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.println(chat_id);

    //Leggo i comandi ed eseguo in base a cosa arriva
    if (text == "/state"){
      Serial.println("1");
      nodemcu.print("1\n");
      String dataIn = "";
      delay(2000);
      while(nodemcu.available() > 0){
        c = nodemcu.read();          
        if(c == '>') {break;}
        else {dataIn+=c;}
      }
      Serial.print(dataIn+"\n");

      if(c == '>'){
        bot.sendMessage(chat_id, dataIn, "");
        c = 0;
        dataIn = "";
      }    
    }else if (text == "/temperature"){
      nodemcu.print("3\n");
      delay(2000);
      String dataIn = "";
      while(nodemcu.available() > 0){
        c = nodemcu.read();          
        if(c == '>') {break;}
        else {dataIn+=c;}
      }

      delay(1000);
      Serial.print(dataIn+"\n");

      if(c == '>'){
        bot.sendMessage(chat_id, dataIn, "");
        c = 0;
        dataIn = "";
      }
    }else if (text == "/terrainhumidity"){
      nodemcu.print("4\n");
      String dataIn = "";
      delay(2000);
      while(nodemcu.available() > 0){
        c = nodemcu.read();          
        if(c == '>') {break;}
        else {dataIn+=c;}
      }

      delay(1000);
      Serial.print(dataIn+"\n");

      if(c == '>'){
        bot.sendMessage(chat_id, dataIn, "");
        c = 0;
        dataIn = "";
      }
    }else if (text == "/airhumidity"){
      nodemcu.print("2\n");
      delay(2000);
      String dataIn = "";
      while(nodemcu.available() > 0){
        c = nodemcu.read();          
        if(c == '>') {break;}
        else {dataIn+=c;}
      }

      delay(1000);
      Serial.print(dataIn+"\n");

      if(c == '>'){
        bot.sendMessage(chat_id, dataIn, "");
        c = 0;
        dataIn = "";
      }
    }else if (text == "/light"){
      nodemcu.print("6\n");
      delay(2000);
      String dataIn = "";
      while(nodemcu.available() > 0){
        c = nodemcu.read();          
        if(c == '>') {break;}
        else {dataIn+=c;}
      }

      delay(1000);
      Serial.print(dataIn+"\n");

      if(c == '>'){
        bot.sendMessage(chat_id, dataIn, "");
        c = 0;
        dataIn = "";
      }
    }else if (text == "/waterlevel"){
      nodemcu.print("5\n");
      String dataIn = "";
      while(nodemcu.available() > 0){
        c = nodemcu.read();          
        if(c == '>') {break;}
        else {dataIn+=c;}
      }

      delay(1000);
      Serial.print(dataIn+"\n");

      if(c == '>'){
        bot.sendMessage(chat_id, dataIn, "");
        c = 0;
        dataIn = "";
      }
    }

    if (text == "/start"){
      String welcome = "Welcome to Greenhouse Bot \n";
      welcome += "/state : get greenhouse information\n";
      welcome += "/irriga : Start irrigation\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup(){
  Serial.begin(57600);
  nodemcu.begin(9600);

  //tentativo di connessione al wifi
  configTime(0, 0, "pool.ntp.org");      
  secured_client.setTrustAnchors(&cert); // certificato di sicurezza per telegram
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

}

void loop(){
  if (millis() - bot_lasttime > BOT_MTBS){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages){
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    
    bot_lasttime = millis();
  }
}
