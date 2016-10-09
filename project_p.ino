/*
Arduino Leonardo <--> ESP8266
Eamil:  earl@microcontrollerelectonics.com
*/

#define SSID "MCMC Guest"
#define PASS "mcmc@tower1"
#define DST_IP "192.168.0.100"
#define CONNECT_ATTEMPTS 5
#define RST 8
#define CHP 9
#define LED 13

#define RESET_PIN 12

void setup() {

  pinMode(RST, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(CHP, OUTPUT);

  reset();

  Serial.println("Starting");
  
  Serial.begin(9600);
  Serial1.begin(115200);

  delay(5000);

  Serial1.println("AT+CWMODE=3");
}

void reset() {
  digitalWrite(LED,HIGH);
  digitalWrite(CHP,HIGH);
  digitalWrite(RST,LOW);
  delay(100);
  digitalWrite(RST,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
}

// reset board
void(* resetFunc) (void) = 0;//declare reset function at address 0

void loop() {
  
  if(initializeWifi()) {
    sendData();
  }

  // sleep for 10 second
  delay(10000);

  Serial.println("Resetting");
//  resetFunc();
}

// initialize ESP8266 connection
boolean initializeWifi() {
  Serial1.println("AT+RST");
  
  delay(1000);

  if (Serial1.find("OK")) { 
    Serial.println("Module is ready");
  } else {
    Serial.println("ESP8266 Module did not respond.");
    return false;
  }
  
  delay(1000);
  boolean connected = false;
  for (int i = 0; i < CONNECT_ATTEMPTS; i++) {

    if (connectWiFi()) {
      connected = true;
      break;
    }
  }
  
  if (!connected) {
    Serial.println("Not connected");
    return false;
  }
  
  delay(5000);
  Serial1.println("AT+CIFSR");
  Serial.println("ip address:");
  while (Serial1.available()) Serial.write(Serial1.read());  

  return true;
}

// Send data to server
void sendData() {
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial1.println(cmd);
  Serial.println(cmd);
  if (Serial1.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  Serial1.print("AT+CIPSEND=");
  Serial1.println(cmd.length());
  if (Serial1.find(">")) {
    Serial.print(">");

  } else {

    Serial.println("connect timeout");
    delay(1000);
    return;
  }
  Serial1.print(cmd);
  delay(2000);
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
    if (c == '\r') Serial.print('\n');
  }

  Serial1.println("AT+CIPCLOSE");
  
  Serial.println("====");
  delay(1000);
}

// Send AT-Command to connect available Wifi
boolean connectWiFi() {
  Serial1.println("AT+CIPMUX=1");
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  Serial.println(cmd);
  Serial1.println(cmd);
  delay(2000);

  if (Serial1.find("OK")) {
    Serial.println("OK, WiFi Connected.");
    return true;
  } else {
    Serial.println("Can not connect to WiFi.");
    return false;
  }
}

