  //  Date: 30/01/2016 - Original code: https://gist.github.com/SamirTafesh/77b58914ad94787ec939

  //  WEB SERVER - Arduino MEGA2560 and ESP8266-01 WIFI module
  //  on MEGA2560 we are using Hardware Serial1 to connect ESP8266 Module (Pins 18 & 19)

  //  For this demonstration, I am connecting: 
  //    3 Leds & 1 Relay As output, To Be Controlled
  //    Web Page to Control these Outputs
    
 HardwareSerial & arduinoTerminal = Serial; 
 HardwareSerial & espSerial = Serial1; 
 

 // set pin numbers for output devices: 
 const int led1 = 11;
 const int led2 = 10;
 const int led3 = 9;
 const int relay1 = 6;
 
 //const int ESP8266_CHPD = 1; 

 // Variables will change: 
 int led1State = LOW;             // ledState used to set the LED1 
 int led2State = LOW;             // ledState used to set the LED2 
 int led3State = LOW;             // ledState used to set the LED3
 int relay1State = LOW;           // Relay State - (Disconnected) 
 
 #define BUFFER_SIZE 256          // Set the Buffer Size (should be updated According to Arduino Board Type) 
 char buffer[BUFFER_SIZE]; 

 void setup() {  
   pinMode(led1, OUTPUT);   
   pinMode(led2, OUTPUT);   
   pinMode(led3, OUTPUT);   
   pinMode(relay1, OUTPUT);   
        
   arduinoTerminal.begin(115200); // Serial monitor 
   espSerial.begin(115200); // ESP8266 
      
   arduinoTerminal.println(F("ESP8266 & Arduino Mega Webserver.")); 

   delay(2000); 
    
   clearSerialBuffer();  //Function to Clear the ESP8266 Buffer 
    
   connectWiFi("SSID", "PASSWORD");  //connect to WIFI router 
   delay(10000);
   
   //test if the module is ready 
   
   arduinoTerminal.print("AT : "); 
   arduinoTerminal.println(GetResponse("AT",100)); 
      
   //Change to mode 1 (Client Mode) 
   arduinoTerminal.print("AT+CWMODE=1 : "); 
   arduinoTerminal.println( etResponse("AT+CWMODE=1",10)); 
          
   //set the multiple connection mode 
   arduinoTerminal.print(F("AT+CIPMUX=1 : ")); 
   arduinoTerminal.println(GetResponse("AT+CIPMUX=1",10)); 
    
   //set the server of port 8888 check "no change" or "OK",  it can be changed according to your configuration 
   arduinoTerminal.print(F("AT+CIPSERVER=1,8888 : ")); 
   arduinoTerminal.println(GetResponse("AT+CIPSERVER=1,8888", 10)); 
   
    //print the ip addr 
   arduinoTerminal.print(F("ip address : ")); 
   arduinoTerminal.println( GetResponse("AT+CIFSR", 20) ); 
   delay(200); 
 
   arduinoTerminal.println(); 
   arduinoTerminal.println(F("Arduino Webserver Started")); 
 
   digitalWrite(led1,led1State);  
   digitalWrite(led2,led2State);  
   digitalWrite(led3,led3State);  
   digitalWrite(relay1,relay1State);  
 } 
 
 void loop() { 
   int ch_id, packet_len; 
   char *pb;   
   espSerial.readBytesUntil('\n', buffer, BUFFER_SIZE); 
    
   if(strncmp(buffer, "+IPD,", 5)==0) { 
     // request: +IPD,ch,len:data 
     sscanf(buffer+5, "%d,%d", &ch_id, &packet_len); 
     if (packet_len > 0) { 
       // read serial until packet_len character received 
       // start from : 
       pb = buffer+5; 
       while(*pb!=':') pb++; 
       pb++; 

        if (strncmp(pb, "GET /?button1", 13) == 0){
          if (led1State == LOW) 
            led1State = HIGH; 
          else 
            led1State = LOW; 
          delay(100); 
          clearSerialBuffer(); 

          digitalWrite(led1, led1State); 
          Serial.println("LED 1"); 
          Serial.println(led1State); 
          homepage1(ch_id); 
        }

        else if (strncmp(pb, "GET /?button2", 13) == 0){
          if (led2State == LOW) 
            led2State = HIGH; 
          else 
            led2State = LOW; 
          delay(100); 
          clearSerialBuffer(); 

          digitalWrite(led2, led2State); 
          Serial.println("LED 2"); 
          Serial.println(led2State); 
          homepage1(ch_id); 
        }

        else if (strncmp(pb, "GET /?button3", 13) == 0){
          if (led3State == LOW) 
            led3State = HIGH; 
          else 
            led3State = LOW; 
          delay(100); 
          clearSerialBuffer(); 

          digitalWrite(led3, led3State); 
          Serial.println("LED 3"); 
          Serial.println(led3State); 
          homepage1(ch_id); 
        }


        else if (strncmp(pb, "GET /?relay1", 12) == 0){
          if (relay1State == LOW) 
            relay1State = HIGH; 
          else 
            relay1State = LOW; 
          delay(100); 
          clearSerialBuffer(); 

          digitalWrite(relay1, relay1State); 
          Serial.println("Relay 1"); 
          Serial.println(relay1State); 
          homepage1(ch_id); 
        }

        else if (strncmp(pb, "GET / ", 6) == 0) { 
          arduinoTerminal.print(millis()); 
          arduinoTerminal.print(" : "); 
          arduinoTerminal.println(buffer); 
          arduinoTerminal.print( "get Status from ch:" ); 
          arduinoTerminal.println(ch_id); 
            
          delay(100); 
          clearSerialBuffer(); 
          homepage1(ch_id); 
       } 
     } 
   } 
   clearBuffer(); 
 } 
 
 String GetResponse(String AT_Command, int wait){ 
   String tmpData; 
    
   espSerial.println(AT_Command); 
   delay(10); 
   while (espSerial.available() >0 )  { 
     char c = espSerial.read(); 
     tmpData += c; 
      
     if (tmpData.indexOf(AT_Command) > -1)          
       tmpData = ""; 
     else 
       tmpData.trim();           
    } 
    return tmpData; 
 } 
 
 void clearSerialBuffer() { 
    while (espSerial.available() > 0) { 
      espSerial.read(); 
    } 
 } 
 
 void clearBuffer() { 
    for (int i =0;i<BUFFER_SIZE;i++ ) { 
      buffer[i]=0; 
    } 
 } 
           
 boolean connectWiFi(String NetworkSSID, String NetworkPASS) { 
   String cmd = "AT+CWJAP=\""; 
   cmd += NetworkSSID; 
   cmd += "\",\""; 
   cmd += NetworkPASS; 
   cmd += "\""; 
    
   arduinoTerminal.println(cmd);  
   arduinoTerminal.println(GetResponse(cmd,10)); 
 } 

  void homepage1(int ch_id) { 
   String Header; 
 
   Header =  "HTTP/1.1 200 OK\r\n"; 
   Header += "Content-Type: text/html\r\n"; 
   Header += "Connection: close\r\n";   
   //Header += "Refresh: 5\r\n"; 
    
   String Content; 

           Content ="<HTML>\r\n";
           Content +="<HEAD>\r\n";
           Content +="<meta name='apple-mobile-web-app-capable' content='yes' />\r\n";
           Content +="<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />\r\n";
           Content +="<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />\r\n";
           Content +="<TITLE>SAM's Home Automation Web Server</TITLE>\r\n";
           Content +="</HEAD>\r\n";
           Content +="<BODY>\r\n";
           Content +="<H1>Home Automation Web Server Arduino</H1>\r\n";
           Content +="<hr />\r\n";
           Content +="<br />\r\n";  
           Content +="<H2>Arduino WIFI (BOSS)</H2>\r\n";
           Content +="<br />\r\n";  
           if (led1State == LOW) 
              Content +="<a href=\"/?button1\"\">LED 1</a><br />\r\n";   
           else 
              Content +="<a style='color:red;' href=\"/?button1\"\">LED 1</a><br />\r\n";   
           Content +="<br />\r\n";     
           Content +="<br />\r\n"; 
           if (led2State == LOW) 
              Content +="<a href=\"/?button2\"\">LED 2</a><br />\r\n";   
           else 
              Content +="<a style='color:red;' href=\"/?button2\"\">LED 2</a><br />\r\n";   
           Content +="<br />\r\n";     
           Content +="<br />\r\n"; 
           if (led3State == LOW) 
              Content +="<a href=\"/?button3\"\">LED 3</a><br />\r\n";   
           else 
              Content +="<a style='color:red;' href=\"/?button3\"\">LED 3</a><br />\r\n";   
           Content +="<br />\r\n";     
           Content +="<br />\r\n"; 
           if (relay1State == LOW) 
              Content +="<a href=\"/?relay1\"\">RELAY 1</a><br />\r\n";   
           else 
              Content +="<a style='color:red;' href=\"/?relay1\"\">RELAY 1</a><br />\r\n";   
           Content +="<br />\r\n";     
           Content +="<br />\r\n"; 
           Content +="<br />\r\n";     
           Content +="<br />\r\n"; 
           Content +="<p>Created by Samir Tafesh. E-mail: samir.tafesh@gmail.com for more projects!</p>\r\n";  
           Content +="<br />\r\n"; 
           Content +="</BODY>\r\n";
           Content +="</HTML>\r\n";
    
   Header += "Content-Length: "; 
   Header += (int)(Content.length()); 
   Header += "\r\n\r\n"; 
    
   espSerial.print("AT+CIPSEND="); 
   espSerial.print(ch_id); 
   espSerial.print(","); 
   espSerial.println(Header.length()+Content.length()); 
   delay(10); 
    
   if (espSerial.find(">")) { 
       espSerial.print(Header); 
       espSerial.print(Content); 
       delay(10); 
    } 
   
 } 
