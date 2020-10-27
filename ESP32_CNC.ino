#include <WiFi.h>
#include "FirebaseESP32.h"
#include <time.h>

#define LED_ROJO 4
#define LED_VERDE 5

#define FIREBASE_HOST "cnc-mobile.firebaseio.com"
#define FIREBASE_AUTH "kREHenkjKnlobINiljU7K6IGY0NMCKepZfiUro42"

FirebaseData firebaseData;

const char *ssid =  "Departamento4B";     // change according to your Network - cannot be longer than 32 characters!
const char *pass =  "MTPS7899A"; // change according to your Network

int timezone = -4 * 3600;
int dst = 0;

String id = "1";
String nameDevice = "CNC-1155848";
String cnc_id = "1";
String dateTime="";
String uid= "";

String comando = "";
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
  Serial.println(ssid);
  Serial.println();
  Serial.println("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.println("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK");   

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, HIGH);
}

void loop(){ 

  while(Serial.available() && i<=0){
    String respuesta = Serial.readStringUntil('\n'); 
    if(respuesta != ""){
      FirebaseJson json;
      json.set("respuesta", respuesta);
      Firebase.pushJSON(firebaseData, "/respuestas/append", json);
    }
  }
  
  if(WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_ROJO, HIGH);
      digitalWrite(LED_VERDE, LOW);
      
  } else{
      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_VERDE, HIGH);
      delay(200);


      //Serial.println(F("===================== GET ROOM FIREBASE================================="));
      
      String mostrar= "/actions/";
      mostrar += cnc_id;
      mostrar += "/command";
      String comandoNuevo = firebaseGetCommand(mostrar);
      if(comandoNuevo != "" && comandoNuevo !=comando){
        comando = comandoNuevo;
        Serial.println(comando);

        //esperamos respuesta
        //enviamos el ok a la app
        //while(Serial.available())
        
        /*while(!Serial.available()){}
        
        String respuesta = Serial.readString(); 
        FirebaseJson json;
        json.set("respuesta", respuesta);
        Firebase.pushJSON(firebaseData, "/respuestas/append", json);
        */

      
        /*
        while(Serial.available() && i<=0){
          String respuesta = Serial.readStringUntil('\n'); 
          FirebaseJson json;
          json.set("respuesta", respuesta);
          Firebase.pushJSON(firebaseData, "/respuestas/append", json);

          //i++;
        }*/
      }

      //Serial.println(F("=================TIME=====================================")); 
  
      time_t now = time(nullptr);
      struct tm* p_tm = localtime(&now);
      int dia=p_tm->tm_mday;
      int mes=p_tm->tm_mon + 1;
      int anio=p_tm->tm_year + 1900;
      int hora=p_tm->tm_hour;
      int minuto=p_tm->tm_min;
      int segundo=p_tm->tm_sec;
      dateTime= "";
      dateTime += dia;
      dateTime += "/";
      dateTime += mes;
      dateTime += "/";
      dateTime += anio;
      dateTime += " ";
      dateTime += hora;
      dateTime += ":";
      dateTime += minuto;
      dateTime += ":";
      dateTime += segundo;
      //Serial.println(dateTime);
      
      /*Serial.println(F("=====================FIREBASE=================================")); 
      Firebase.setString("/access-elevator/"+id+"/id", id);
      Firebase.setString("/access-elevator/"+id+"/name", nameDevice);
      Firebase.setString("/access-elevator/"+id+"/datetime", dateTime);
      Firebase.setString("/access-elevator/"+id+"/room_id", room_id);
      Firebase.setString("/access-elevator/"+id+"/rfid", uid);
      */


      //Serial.println(F("=======================SHOW DATA==============================="));
      //Serial.println("id: "+id+" name: "+nameDevice+" datetime: "+dateTime+" room_id: "+room_id+" rfid: "+uid);
      //Serial.println(F("=======================FIN===============================")); 
  
    
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
  Firebase.pushJSON(firebaseData, "/respuestas/append", json);

}
