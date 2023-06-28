#include <SPI.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>

SoftwareSerial nodemcu(5, 6);

#define RST_PIN         7           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

#define WR_BTN 2

#define RFID_READ_SUCCESS 4

byte NUM_ATMP=10;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

LiquidCrystal_I2C lcd(0x27, 16, 2);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

byte sector         = 1;
byte blockAddr      = 2;

    /**
byte dataBlock[]    = {
  0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
  0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
  0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
  0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
};*/
byte dataBlock = {"Momose Narumi"};

byte buffer[18];
byte size = sizeof(buffer);

char blockContent[17];

double temperature = 36;

/**
 * Initialize.
 */
void setup() {
    pinMode(WR_BTN, INPUT);
    pinMode(RFID_READ_SUCCESS, OUTPUT);
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    nodemcu.begin(9600);

    lcd.begin();

    lcd.backlight();
    lcd.clear();
    lcd.print("Scan your card");

    if(!mlx.begin()){
      Serial.println("Error connecting to MLX");
    }

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}

/**
 * Main loop.
 */
void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }
    MFRC522::StatusCode status;
    // Read data from the block
    StaticJsonDocument<1000> data;
    readMode();
    temperature = mlx.readObjectTempC();
    data["rfid_code"] = blockContent;
    data["temperature"] = temperature;
    serializeJson(data, nodemcu);
//    fromNode();
    Serial.println("");
    serializeJson(data, Serial);
    
//    writeRFID();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}
void fromNode(){
 String IncomingString="";
 boolean StringReady = false;
 
 while (nodemcu.available()){
   IncomingString=nodemcu.readString();
   StringReady= true;
  }
 
  if (StringReady){
    Serial.print("Node: ");
    Serial.println(IncomingString);
  }
}
void writeRFID(){
  byte res = writeBlock(blockAddr, dataBlock);
  if(res == 69){
    lcd.clear();
    lcd.print("Scan your card");
    delay(1000);
  }else Serial.println("Failed to write");
  lcd.clear();
  lcd.print("Writing to card");
}
void readMode(){
    digitalWrite(RFID_READ_SUCCESS, HIGH);
    readBlock(blockAddr, buffer);
    getContentAsString();
    lcd.clear();
    lcd.print("Success");
    lcd.setCursor(0,1);
    lcd.print(blockContent);
    delay(2000);
    lcd.clear();
    lcd.print("Scan your card");
    digitalWrite(RFID_READ_SUCCESS, LOW);
}
void writeMode(){
      Serial.println("Wite Mode");
      lcd.clear();
      lcd.print("Writing to card");
      byte attempt = 0;
      while(attempt < NUM_ATMP){
        // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()){
          byte res = writeBlock(blockAddr, dataBlock);
            if(res == 69){
              Serial.println("Write Success");
              lcd.clear();
              lcd.print("Scan your card");
              delay(1000);
              break;
            }else Serial.println("Failed to write");
        }
        Serial.print("Attempt: ");
        Serial.print(attempt);
        Serial.println(" Failed");
        attempt++;
        delay(500);
      }
      Serial.println("Finished Writing");
      return;
}
// get content of a block as a string
void getContentAsString(){
  readBlock(blockAddr, buffer);
  memcpy(blockContent, buffer, size);
  blockContent[16] = '\0';
}
//Read specific block
int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
         return 3;//return "3" as error message
  }

//reading a block
byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          return 4;//return "4" as error message
  }
}

//Write specific block    
int writeBlock(int blockNumber, byte arrayAddress[]) 
{
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}
  
  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         return 3;//return "3" as error message
  }
  
  //writing the block 
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
           return 4;//return "4" as error message
  }
  return 69;
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
