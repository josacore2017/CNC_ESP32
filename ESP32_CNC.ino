#include <WiFi.h>
#include "FirebaseESP32.h"

#define LED_ROJO 4
#define LED_VERDE 5

#define FIREBASE_HOST "cnc-mobile.firebaseio.com"
#define FIREBASE_AUTH "kREHenkjKnlobINiljU7K6IGY0NMCKepZfiUro42"

FirebaseData firebaseData;

const char *ssid =  "Departamento4B";     // change according to your Network - cannot be longer than 32 characters!
const char *pass =  "MTPS7899A"; // change according to your Network

String id = "1";
String nameDevice = "CNC-1155848";
String cnc_id = "1";
String dateTime="";
String uid= "";

String comando = "";
String tiempo ="";
String res = "";
int i=0;

void printResult(FirebaseData &data);

void setup() {

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  
  Serial.begin(115200);
  delay(250);
  
  WiFi.begin(ssid, pass);
  
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 10)) {
    retries++;
    delay(500);
    Serial.print(".");
    digitalWrite(LED_ROJO, HIGH);
    digitalWrite(LED_VERDE, LOW);
  }
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  while(!time(nullptr)){
     delay(1000);
  }  

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, HIGH);
}

void loop(){
  /*
  if(Serial.available()>0)    //Checks is there any data in buffer 
  {
    //Serial.print(char(Serial.read()));  //Read serial data byte and send back to serial monitor
      if(char(Serial.read())=="o"){
      addDataOK(tiempo);
    }
  }
  */

  String readString="";
  delay(10);
  while (Serial.available()) {
    delay(10); 
    if (Serial.available() >0) {
      char c = Serial.read();
      readString += c;
    }
  }
  if (readString.length() >2) {
    addDataOK(tiempo);
  }




  
  if(WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_ROJO, HIGH);
      digitalWrite(LED_VERDE, LOW);
      
  } else{
      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_VERDE, HIGH);
      delay(200);

      String mostrar= "/commands/";
      mostrar += cnc_id+"/1";
      mostrar += "/command";
      String comandoNuevo = firebaseGetCommand(mostrar);
      if(comandoNuevo != "" && comandoNuevo !=comando){
        String tiempoData = getValue(comandoNuevo,'|',0);
        String comandoData = getValue(comandoNuevo,'|',1);
        comando = comandoNuevo;
        tiempo = tiempoData;
        Serial.println(comandoData); 
      }
  }
}

String firebaseGetCommand(String s){
  String x ="";
  if (Firebase.getString(firebaseData, s)) {
    x = firebaseData.stringData();
  }
  yield();
  delay(100);
  return x;
  delay(100);
    
}
void addDataOK(String respuesta){
  FirebaseJson json;
  json.set("respuesta", respuesta);
  Firebase.pushJSON(firebaseData, "/respuestas/append/"+cnc_id, json);
  Firebase.setString(firebaseData, "/devices/"+cnc_id+"/lastTime", respuesta);
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
