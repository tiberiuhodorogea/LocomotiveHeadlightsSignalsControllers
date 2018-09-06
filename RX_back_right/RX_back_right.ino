#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>

//ROLE SPECIFIC CODE
#define FRONT_PIPE 0xF0F0F0F0C1LL
#define BACK_PIPE  0xF0F0F0F0CALL
//END ROLE SPECIFIC CODE

//COMMON SETTINGS FOR EVERYONE
#define ACK_RETRIES 15
#define ACK_RETRY_DELAY_MICROS 2500
#define CHANNEL 115
//END COMMON SETTINGS FOR EVERYONE

#define FRONT 0
#define BACK 1

#define RED_LED_PIN 2
#define BLUE_LED_PIN 3
#define WHITE_LED_PIN 4

#define LIGHT_BLUE_BIT 1
#define LIGHT_RED_BIT  2
#define LIGHT_WHITE_BIT  3
typedef byte Package;

RF24 radio(7, 8);
int rec[1] = {2};

const uint64_t pipes[2] = {FRONT_PIPE, BACK_PIPE};

Package package;
void setup()
{
  Serial.begin(115200);
  radio.begin();
  delay(50);
  Serial.println("Starting...");
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(WHITE_LED_PIN, OUTPUT);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setChannel(CHANNEL);
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipes[FRONT]);
  radio.openReadingPipe(2, pipes[BACK]);
  radio.writeAckPayload( 1, rec, sizeof(int) );
  radio.startListening();
  radio.setRetries(ACK_RETRY_DELAY_MICROS / 250, ACK_RETRIES);
}

void loop()
{
  uint8_t whichPipeAvailable;
  if ( radio.available(&whichPipeAvailable) ) {
    radio.writeAckPayload( 1, rec, sizeof(int) );
    radio.read( &package, sizeof(package) ); rec[0] += 2;
    Serial.println("Command got is : " + String(package) + ", from: " +( whichPipeAvailable == 1 ? "FRONT" :"BACK" ) + " Executing...");
    executeCommand(package);
  }
}

void executeCommand(Package command)
{
  if ((package >> LIGHT_RED_BIT) & 1)
  {
    logLight("RED", true);
    analogWrite(RED_LED_PIN, 255);
  }
  else
  {
    logLight("RED",false);
    analogWrite(RED_LED_PIN, 0);
  }

  if ((package >> LIGHT_BLUE_BIT) & 1)
  {
    logLight("BLUE",true);
    analogWrite(BLUE_LED_PIN, 255);
  }
  else
  {
    logLight("BLUE",false);
    analogWrite(BLUE_LED_PIN, 0);
  }

  if ( (package >> LIGHT_WHITE_BIT ) & 1 )
  {
     logLight("WHITE",true);
    analogWrite(WHITE_LED_PIN, 255);
  }
  else
  {
    logLight("WHITE",false);
    analogWrite(WHITE_LED_PIN, 0);
  }
}

void logLight(char* color, bool light){
  Serial.println(color + String(light));
}

