#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 9
#define SS_PIN 10

#define BTN 3

LiquidCrystal_I2C lcd(0x27, 16, 4);

byte readCard[4];
String MasterTag = "20C3935E";  // REPLACE this Tag ID with your Tag ID!!!
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;    // create the mifare struct, which will hold the card information
// the block where we would write and read to
byte block = 2;

//an array with 16 bytes to be written into one of the 64 card blocks is defined
byte contentToWriteToBlock[16] =  {"736587364856"};

// this is an array that holds the data read from a block
byte contentReadFromABlock[18];

String blockContent = "";
String contentToWrite = "";

void setup() 
{
  pinMode(BTN, INPUT);
  Serial.begin(9600);
  SPI.begin();  // init SPI bus
  mfrc522.PCD_Init();     // init the mfrc card
  // initialize the screen
  lcd.begin();

  // turn on backligth
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Scan your card");

  // prepare the security key for read and write functions
  for(byte i = 0; i < 6; i++){
    key.keyByte[i] = 0xFF;    // keyByte is defined in the "MIFARE_KEY" 'struct' definition
  }
}

void loop() 
{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Writing on card");
    // Getting ready for Reading PICCs
      if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
      return false;
      }
      if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
      return false;
      }
      writeBlock(block, contentToWriteToBlock);
      lcd.setCursor(0,1);
      lcd.print("Done!!");
      delay(1000);
}

//Write specific block    
int writeBlock(int blockNumber, byte arrayAddress[]) 
{
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}
  Serial.print(blockNumber);
  Serial.println(" is a data block:");
  
  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed: ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;//return "3" as error message
  }
  
  //writing the block 
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
           Serial.print("MIFARE_Write() failed: ");
           Serial.println(mfrc522.GetStatusCodeName(status));
           return 4;//return "4" as error message
  }
  Serial.println("block was written");
}

// get content of a block as a string
void getContentAsString(){
  blockContent = "";
  readBlock(block, contentReadFromABlock);
  for(byte i = 0; i < 18; i++){
    blockContent.concat(String(contentReadFromABlock[i], HEX));
  }
}

void writeContentAsString(){
  for(byte i = 0; i < 16; i++){
    contentToWriteToBlock[i] = contentToWrite[i];
  }

  writeBlock(block, contentToWriteToBlock);
}

//Read specific block
int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(mfrc522.GetStatusCodeName(status));
         return 3;//return "3" as error message
  }

//reading a block
byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println(mfrc522.GetStatusCodeName(status));
          return 4;//return "4" as error message
  }
  Serial.println("block was read");
}
//Read new tag if available
boolean getContent() 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  getContentAsString();
  return true;
}
/**
boolean getID() 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}*/

// convert the data from a block to a string
