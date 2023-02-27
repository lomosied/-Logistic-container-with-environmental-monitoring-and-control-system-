#include <LiquidCrystal.h>  // Include LiquidCrystal Library
#include <SoftwareSerial.h>
#include <dht.h>

#define dhtPin 8
const int MQpin=A0;//the AOUT pin of the alcohol sensor goes into analog pin A0 of the arduino
#define sensorPin1 A1

int i,alco_high,alco_low,mapped,data[200];

SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #11 & #10 
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // Create an LCD object.
dht DHT;    // Create a DHT object

const int buzzer = 9; //buzzer to arduino pin 9
int Relaypin= 12; // Define input pin for relay
float h;
float t;
float temperatureC;
String message;
String condition;
String containerNumber;

void setup() {
  lcd.begin(16,2); // Initialize the LCD
  Serial.begin(9600);
  
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  containerNumber = "135_G";
pinMode(Relaypin, OUTPUT); // Define the Relaypin as output pin
  pinMode(MQpin, INPUT);//sets the pin as an input to the arduino
for(i=2;i<=5;i++)
{
  pinMode(i,OUTPUT);//sets the pin as an output of the arduino
}

}

void loop() {
////MQ sensor
  alco_high=0; // initially alcohol level is zero
  alco_low = 1023; // low threshold is 1023

        for(i=200; i>0; i--){
        data[i] = data[i-1]; // decrement of data
        
      if(data[i]>alco_high)
      alco_high=data[i];
      if(data[i]<alco_low)
      alco_low=data[i];
      }
data[0]= analogRead(MQpin);//reads the analaog value from the alcohol sensor's AOUT pin
mapped=map(data[0],0,1023,0,600);
Serial.print("Alcohol value: ");
Serial.println(mapped);//prints the alcohol value
//delay(100);


  
  int readData = DHT.read11(dhtPin);
  int tmp36reading = analogRead(sensorPin1);
  // Convert that reading into voltage
  // Replace 5.0 with 3.3, if you are using a 3.3V Arduino
  float voltage = tmp36reading * (5.0 / 1024.0);
  // Convert the voltage into the temperature in Celsius
   temperatureC = (voltage - 0.5) * 100;

  // Print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print("\xC2\xB0"); // shows degree symbol
  Serial.print("C  |  ");
  
   t = DHT.temperature;
   h = DHT.humidity;
lcdDisplay();

if (temperatureC > 100){
  Serial.println("High product temp");
  condition =  "High product temp";
  highProductTemp();
  buzzerSound();
  sendMessage();
  digitalWrite(Relaypin, HIGH); // Sends high signal 
  }
  else if(t > 35){
    Serial.println("High container temperature");
    condition = "High container Temp";
    highContainerTemp();
    buzzerSound();
    sendMessage();
    digitalWrite(Relaypin, HIGH); // Sends high signal
    }
    else if (h<30){
      Serial.println("Low Humidity");
      condition = "Low humidity";
      lowHumidity();
      buzzerSound();
      }
      else if (mapped > 350){
        Serial.println("Ethanol detected|Signs of fermentation");
        condition = "Ethanol detected";
        alcoholDetected();
        buzzerSound();
        sendMessage();
        digitalWrite(Relaypin, HIGH); // Sends high signal
        }
else {
  digitalWrite(Relaypin, LOW); // Makes the signal low
  }
message = "Container No:" + containerNumber + "\n Detected condition: " + condition + "\n Control mechanism initiated," + "\n Instruct transporter to inspect.";
}

void lcdDisplay()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("H:");
  lcd.print(int(h));//convert float values to int to remove decimals
  lcd.print("%");
  
  lcd.setCursor(6,0);
  lcd.print("cT:");
  lcd.print(int(t));//convert float values to int to remove decimals
  lcd.print((char)223);//shows degrees character
 // lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("pT:");
  lcd.print(int(temperatureC));//convert float values to int to remove decimals
  lcd.print((char)223);//shows degrees character
  //lcd.print("C");

  lcd.setCursor(8,1);
  lcd.print("A:");
  lcd.print(mapped);//convert float values to int to remove decimals
  //lcd.print((char)223);//shows degrees character
  //lcd.print("C");

  
  delay(500);  
}
void alcoholDetected(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alcohol Detected");
  lcd.setCursor(0,1);
  lcd.print("Alc Cont:");
  lcd.print(mapped);

  
  }
  void highProductTemp(){
    lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("High Product Temp");
  lcd.setCursor(0,1);
  lcd.print("Product Temp:");
  lcd.print(int(temperatureC));
    }

  void highContainerTemp(){
       lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("High Cont Temp");
  lcd.setCursor(0,1);
  lcd.print("Cont Temp:");
  lcd.print(int(t)); 
  
    }

   void lowHumidity(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Low Humidity");
  lcd.setCursor(0,1);
  lcd.print("Humi:");
  lcd.print(int(h));  
    }

  void buzzerSound(){
      tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(100);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(100);        // ...for 1sec
    }
void sendMessage(){
    //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);  
  //Begin serial communication with Arduino and SIM800L
     mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
 mySerial.println("AT+CMGS=\"+254746685295\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  
  mySerial.print(message); //text content
  updateSerial();
  mySerial.write(26);
  }

    
  void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
