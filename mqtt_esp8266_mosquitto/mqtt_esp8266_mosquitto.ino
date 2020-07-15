
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "Red_Wifi_De_Carlos";
const char* password = "xxxxx";  // passWifi
const char* mqtt_server = "99999"; // ip server 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
byte estadoPulsoBinario = 0; 
char valueStr[15];
//char topicSalaLed[15];
//char topicSalaSensor[15];

int estadoPinSensor =0;
int estadoPinBoton =0;
String strSensorUltimo;
String strSensor;

String strBotonUltimo;
String strBoton;


#define D1 05  // lector de sensor 
#define D2 04  // led de sensor 

#define D3 0   //   
#define D5 14  // lector de boton
#define D6 12  // 
#define D7 13  // 

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character

 
  if (String (topic) == "home/salaLed") {   
    //Serial.println((char)payload[1]);    
    
    if ((char)payload[1] == 'N') {
       digitalWrite(D6, HIGH);       
    }
    else 
    {
      digitalWrite(D6, LOW);
    }
  }

  if (String (topic) == "home/salaSensor") {   
    if ((char)payload[1] == 'N') 
    {
       digitalWrite(D2,HIGH);
       delay(2000);
       digitalWrite(D2,LOW);
    }
     else 
    {    
     digitalWrite(D2,LOW);    
    }
  }

  
  

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"carlosf", "carlosf"))
    { 
      Serial.println("connected");
      // Once connected, publish an announcement...
      
      client.subscribe("home/salaSensor");
      client.subscribe("home/salaLed");
      
      client.publish("home/salaSensor", "hello world home/salaSensor");
      client.publish("home/salaLed", "hello world home/salasalaLed");
      // ... and resubscribe
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  pinMode(D1, INPUT); // D1
  pinMode(D2, OUTPUT); // D2

  pinMode(D5, INPUT); // D1
 // pinMode(D5, OUTPUT); // D2

  pinMode(D6, OUTPUT); // D1

  pinMode(D7, INPUT); // D1

  digitalWrite(D6,LOW); 



   
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  client.setCallback(callback);
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //----------------------------------------------------------------------
  //Sensor 
  estadoPinSensor = digitalRead(D1);  
  if (estadoPinSensor == HIGH)
  {    
    strSensor = "ON";   
  }
  else 
  {
    strSensor = "OFF";    
  }


   if (strSensor != strSensorUltimo) 
  { //envia el estado del pulsador solamente cuando cambia.
    strSensorUltimo = strSensor;
    strSensor.toCharArray(valueStr, 15);
    Serial.println("Enviando Sensor: " + strSensor);
    client.publish("home/salaSensor", valueStr);

            
  }
  //----------------------------------------------------------------------------
  
  // Boton Activa Led
   
  estadoPinBoton = 0;
    
  estadoPinBoton = digitalRead(D5);  

  //Serial.print(estadoPinBoton);
  
  if (estadoPinBoton == 1)
  {    
    strBoton = "ON";        
  }
  else 
  {
    strBoton = "OFF";        
  }
  
  if (strBoton != strBotonUltimo) 
  { //envia el estado del pulsador solamente cuando cambia.
    
    strBotonUltimo = strBoton;

    if (strBoton =="ON")
    {
       if (estadoPulsoBinario != 1)  
        {
          //digitalWrite(D6, HIGH);        
          strBoton = "ON";
          estadoPulsoBinario = 1;
        }
        else 
        {
          strBoton = "OFF";
          estadoPulsoBinario = 0;
          //digitalWrite(D6, LOW);        
        }


         strBoton.toCharArray(valueStr, 15);
         Serial.println("Enviando Boton: " + strBoton);
         client.publish("home/salaLed", valueStr);
    }
    
   

           
  }

   
}
