#include <SD.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 


File root;

///////Variabili Setup AP
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)
int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

//Variabili globali sito e codice
String html;
char command_html;
char command_code;


String line = "";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

   Serial.print("Initializing SD card...");

  if (!SD.begin(7)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }



  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
  changePage(command_html);
}


void loop() {
  
  //SetupWifi();
  //changePage(command_html);
  RefreshPage(); 
  DoCode(command_code);
  

 
}

void SetupWifi(){
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

void RefreshPage(){
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(html);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        
        
        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /N")) {
          command_html = 'N';
          command_code = 'H'; // LED ALTO
          changePage(command_html);
          
        }
        if (currentLine.endsWith("GET /B")) {
          command_html = 'B';
          command_code = 'L';
          changePage(command_html);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }


}






void changePage(char command){
String startpage ="<!DOCTYPE html> <html> <head> <title>Prova</title> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\"> <link rel=\"stylesheet\" href=\"style.css\"> </head> <body> <div class=\"row\"> <div class= col-4 style=\"background-color: #E6E6E6 ; \"> <div class=\"table-wrapper-scroll-y my-custom-scrollbar\" style=\"display: block;position: relative;height: 400px;overflow: auto;\"> <table class=\"table\"> <thead> <tr> <th>#</th> <th><i></i>FileName</th> <th><i></i>Actions</th> </tr> </thead> <tbody>";

String buttonGreen = "<a href=\"N\" type=\"button\" class=\"btn btn-outline-success\" style=\"width: 450px\">NewFileStart</a> </div> </div> <script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\"></script> <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script> </body> </html>";
String buttonRed = "<a href=\"B\" type=\"button\" class=\"btn btn-outline-danger\" style=\"width: 450px\">EndFile</a> </div> </div> <script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\"></script> <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script> </body> </html>";

int cnt=0;
  //Create the list file
  root = SD.open("/");
  printDirectory(root, 0);
  root.close();

  
  
  
  
  
  //Create the tab
  File list = SD.open("LIST.TXT");
  while (list.available()) {
    line = list.readStringUntil('\n');
    if(line.startsWith("DATA")){
      cnt +=1;
      startpage += "<tr> <th scope=\"row\">";
      startpage += String(cnt);
      startpage += "</th> <td><i></i>";
      startpage += line;
      startpage += "</td>"; 
      startpage += "<td> <a href=\"";
      startpage += line;
      startpage += "-D";
      startpage += "\"><img src=\"https://image.flaticon.com/icons/svg/126/126488.svg\" style=\"height: 25px;width: 25px;\"></img></a> <a href=\"";
      startpage += line;
      startpage += "-R";
      startpage += "\"><img src=\"https://image.flaticon.com/icons/svg/214/214356.svg\" style=\"height: 25px;width: 25px;\"></img></a> </td> </tr> ";
  
    }
    
  }
  list.close();
  
  
  startpage +="</tbody> </table> </div>";

  switch(command){
  case 'N':
      html = startpage + buttonRed;
      break;
   
  case 'B':  
      html = startpage + buttonGreen;
      break;

  default:
    html = startpage + buttonGreen;
    break; 
  }
}



void DoCode(char command){
  switch(command){
    case 'H':
        digitalWrite(led, HIGH);
        break; 
    case 'L':
        digitalWrite(led, LOW);
        break;
    default:
        break;
  }
}

void printDirectory(File dir, int numTabs) {
String tmp;

  if(SD.exists("list.txt")){
    SD.remove("list.txt");
    }
  
  File list;
  list = SD.open("list.txt",FILE_WRITE);
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      list.close();
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      list.print('\t');
    }
    list.println(entry.name());
    if (entry.isDirectory()) {
      list.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
    
    }
    entry.close();
  }
}
