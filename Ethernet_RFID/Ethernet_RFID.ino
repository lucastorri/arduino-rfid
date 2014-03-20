/*
Pins:
 
           D00  D01  D02  D03  D04  D05  D06  D07  D08  D09  D10  D11  D12  D13  A00  A01  A02  A03  A04  A05
       LCD                                                                                           SDA  SDL
      RFID                                         RST  SDA      MOSI MISO  SCK 
     DHT11                  S
   RED LED                                 X
 GREEN LED                                      X
     RELAY                                      S
 
*/

#define RS232_BAUD 9600

#define HTTP_SERVER_PORT 80

#define RFID_SS_PIN 9
#define RFID_RST_PIN 8
#define RFID_VALID "4463913F"

#define GREEN_PIN 6
#define RED_PIN 7

#define LCD_SIZE 16,2

#define DHT11_PIN 3
#define DHT11_INTERRUPTION 1

#define RESULT_INTERVAL 3000
#define WARMUP_INTERVAL 1000

#include <Scheduler.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <idDHT11.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

typedef enum State {
  START,
  NORMAL,
  RED,
  GREEN
} State;

struct Content {
  float temperature;
  float humidity;
  char uid[21];
};

EthernetServer server(HTTP_SERVER_PORT);
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
idDHT11 DHT11(DHT11_PIN, DHT11_INTERRUPTION, dht11_int_handler);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Content content;
Scheduler scheduler;

void setup() {
  static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  
  Serial.begin(RS232_BAUD);
  SPI.begin();
  
  lcd.begin(LCD_SIZE);
  lcd.backlight();
  
  rfid.PCD_Init();
  content.uid[0] = '\0';
  
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at "); Serial.println(Ethernet.localIP());
  
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  updateUI(START);
  
  scheduler.after(WARMUP_INTERVAL, normalBehaviour);
  scheduler.every(0, replyRequests, WARMUP_INTERVAL);
}

void loop() {
  scheduler.run();
}

void updateTemperature() {
  DHT11.acquireAndWait();
  if (temperatureChanged()) {
    content.temperature = DHT11.getCelsius();
    content.humidity = DHT11.getHumidity();
    updateUI(NORMAL);
  }
}

void readCard() { 
  if (cardAvailable()) {    
    scheduler.cancel(updateTemperature);
    scheduler.cancel(readCard);
    
    isUidValid() ? updateUI(GREEN) : updateUI(RED);
    
    scheduler.after(3000, normalBehaviour);
  }
}

void normalBehaviour() {
  updateUI(NORMAL);
  scheduler.every(5000, updateTemperature, 0);
  scheduler.every(0, readCard);
}

void updateUI(int state) {
  digitalWrite(GREEN_PIN, state == GREEN);
  digitalWrite(RED_PIN, state == RED);

  lcd.clear();  
  if (state == START) {
    lcd.print("booting up...");
  } else if (state == NORMAL) {
    lcd.print("T: "); lcd.print(content.temperature); lcd.print("oC");
    lcd.setCursor(0, 1);
    lcd.print("H: "); lcd.print(content.humidity); lcd.print("%");
  } else if (state == GREEN) {
    lcd.print("Hi there!");
  } else if (state == RED) {
    lcd.print("unknown...");
  }
}

boolean temperatureChanged() {
  return content.temperature != DHT11.getCelsius() || content.humidity != DHT11.getHumidity();
}

boolean cardAvailable() {
 return rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial();
}

void replyRequests() {
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          htmlResponse(&client);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

boolean isUidValid() {
  for (int i = 0; i < rfid.uid.size; i++) {
    sprintf(&content.uid[i * 2], "%02X", rfid.uid.uidByte[i]);
  }
  content.uid[(rfid.uid.size * 2) + 1] = '\0';
  boolean isValid = !strcmp(RFID_VALID, content.uid);
  
  return isValid;
}

void htmlResponse(EthernetClient *client) {
  client->println("HTTP/1.1 200 OK");
  client->println("Content-Type: text/html");
  client->println("Connection: close");
  client->println();
  client->println("<!DOCTYPE HTML>");
  client->println("<html><body>");
  
  client->print("Last user: ");
  client->print(content.uid);
  client->println("<br />");
  
  client->print("Temperature: ");
  client->print(content.temperature);
  client->print("oC");
  client->println("<br />");
  
  client->print("Humidity: ");
  client->print(content.humidity);
  client->print("%");
  client->println("<br />");
  
  client->println("</body></html>");
}

void dht11_int_handler() {
  DHT11.isrCallback();
}
