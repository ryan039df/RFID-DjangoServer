#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//#define SERVER_IP "10.252.246.135"

#ifndef STASSID
#define STASSID "CALVIN-Student"
#define STAPSK "CITStudentsOnly"
#endif

constexpr uint8_t RST_PIN = 5;
constexpr uint8_t SS_PIN = 4;

String phrase_1;
String phrase_2;
String phrase_3;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
  }
  Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
  Serial.print(F("Using key (for A and B):"));
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println();

  Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent())
      return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
      return;
  
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    //________________________________________________________________________________________________________________
    // Getting the RFID data that will be send to server
    
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
  
    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }
  
    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector           = 1;
    byte blockAddr_1      = 6;
    byte blockAddr_2      = 5;
    byte blockAddr_3      = 4;
  
    // data in byte array
    byte dataBlock_1[]    = {
        0x49, 0x44, 0x3A, 0x30, //  1,  2,   3,  4,
        0x30, 0x31, 0x30, 0x30, //  5,  6,   7,  8,
        0x32, 0x33, 0x34, 0x2C, //  9, 10, 255, 11, // "ID:00100234, "
        0x20, 0x20, 0x20, 0x20  // 12, 13, 14, 15   // "    "
    };
  
    byte dataBlock_2[]    = {
        0x44, 0x61, 0x74, 0x65, //  1,  2,   3,  4,
        0x3A, 0x31, 0x37, 0x30, //  5,  6,   7,  8,
        0x38, 0x34, 0x35, 0x2C, //  9, 10, 255, 11, // "Date:170845,"
        0x20, 0x20, 0x20, 0x20  // 12, 13, 14, 15   // "    "
    };
    
    byte dataBlock_3[]    = {
        0x50, 0x72, 0x6F, 0x64, //  1,  2,   3,  4,
        0x75, 0x63, 0x74, 0x3A, //  5,  6,   7,  8,
        0x58, 0x20, 0x20, 0x20, //  9, 10, 255, 11, // "Product:X   "
        0x20, 0x20, 0x20, 0x20  // 12, 13, 14, 15   // "    "
    };
    
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
  
    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
  
    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();
  
    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
  
    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr_1);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock_1, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr_1, dataBlock_1, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
  
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr_2);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock_2, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr_2, dataBlock_2, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
  
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr_3);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock_3, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr_3, dataBlock_3, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();
  
    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Reading data from a block and convert it to string from byte
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr_1);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr_1, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr_1); Serial.println(F(":"));
//    dump_byte_array(buffer, 16); Serial.println();

    phrase_1 = "";
    for (int i =0; i<16; i++)
    {
      phrase_1 = String(phrase_1 + (char)buffer[i]);
    }
    phrase_1.remove(12, 4); // ID:00100234,


    // Reading data from a block and convert it to string from byte
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr_2);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr_2, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr_2); Serial.println(F(":"));
//    dump_byte_array(buffer, 16); Serial.println();
    phrase_2 = "";
    for (int i =0; i<16; i++)
    {
      phrase_2 = String(phrase_2 + (char)buffer[i]);
    }
    phrase_2.remove(12, 4); // ID:00100234,


    // Reading data from a block and convert it to string from byte
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr_3);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr_3, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr_3); Serial.println(F(":"));
//    dump_byte_array(buffer, 16); Serial.println();
    phrase_3 = "";
    for (int i =0; i<16; i++)
    {
      phrase_3 = String(phrase_3 + (char)buffer[i]);
    }
    phrase_3.remove(9, 7);  // Product:X
    
    Serial.print("{\"value\":\"" + phrase_1 + " " + phrase_2 + " " + phrase_3 + "\", " + "\"reader_id\":\"" + "A1" + "\"}");
    Serial.println();
    
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    //________________________________________________________________________________________________________________

    // Send data to server
    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    bool res = http.begin(client, "http://10.252.246.135:8000/rfid-data/");  // HTTP (USE DEVICE IP, don't use localhost)
    Serial.println(res);
    http.addHeader("Content-Type", "application/json");
    Serial.print("[HTTP] POST...\n");
    
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"value\":\"" + phrase_1 + " " + phrase_2 + " " + phrase_3 + "\", " + "\"reader_id\":\"" + "A2" + "\"}");
  
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } 
    
    else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(1000);
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i], HEX);
    }
}
