#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>

//ROLE SPECIFIC CODE
//vector positions - THESE ARE RELATIVE TO THE TRANSMITTER
#define RX_FRONT_LEFT 0
#define RX_FRONT_RIGHT 1
#define RX_BACK_LEFT 2
#define RX_BACK_RIGHT 3

//!!PIPES ARE DIFFRENT PER TRANSMITTER
const uint64_t pipes[4] = {0xF0F0F0F0E1LL, 0xF0F0F0F0F1LL, 0xF0F0F0F0A1LL, 0xF0F0F0F0C1LL};
//END ROLE SPECIFIC CODE - FROM DOWN BELOW SHOULD BE THE SAME ROLE

//COMMON SETTINGS FOR EVERYONE
#define ACK_RETRIES 15
#define ACK_RETRY_DELAY_MICROS 2500
#define CHANNEL 115
//END COMMON SETTINGS FOR EVERYONE

int msg[1] = {1};
int rec[1] = {5};

#define VARIANT_ZERO_PIN 5
#define VARIANT_ONE_PIN 4 // WHITE BUTTON
#define VARIANT_TWO_PIN 3 // BLUE BUTTON
#define VARIANT_THREE_PIN 2  // RED BUTTON

#define LIGHT_BLUE_BIT 1
#define LIGHT_RED_BIT  2
#define LIGHT_WHITE_BIT  3


//GLOBALS
//
RF24 radio(7, 8);

bool activeAction = false;

bool variantZeroSelected = false;
bool variantOneSelected = false;
bool variantTwoSelected = false;
bool variantThreeSelected = false;

//END GLOBALS
typedef byte Package;

void initRadio() {
  radio.begin();
  delay(50);
  radio.setAutoAck(true);
  radio.setChannel(CHANNEL);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.stopListening();
  radio.setRetries(ACK_RETRY_DELAY_MICROS / 250, ACK_RETRIES);
}

void initPins() {
  pinMode(VARIANT_ZERO_PIN, INPUT);
  pinMode(VARIANT_ONE_PIN, INPUT);
  pinMode(VARIANT_TWO_PIN, INPUT);
  pinMode(VARIANT_THREE_PIN, INPUT);
}
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");
  initRadio();
  initPins();
}

const char * getReceiverName(const uint64_t pipe) {
  switch (pipe) {
    case RX_FRONT_LEFT:
      return "FRONT LEFT";
    case RX_FRONT_RIGHT:
      return "FRONT RIGHT";
    case RX_BACK_LEFT :
      return "BACK LEFT";
    case RX_BACK_RIGHT:
      return "BACK RIGHT";
    default:
      return "ERROR IN SWITCH BY PIPE, INVALID VALUE";
  }
}

void sendToReceiver(const uint64_t pipe, const Package *package)
{
  radio.openWritingPipe(pipe);
  Serial.println(String("Sending to ") + String((long)pipe));
  if (radio.write(package, sizeof(Package)))
  {
    Serial.print( "Sent success: " + String(*package) );
    if (radio.isAckPayloadAvailable())
    {
      Serial.println("isAckPayloadAvailable = true ");
      while (radio.available())
      {
        radio.read(rec, sizeof(int));
        Serial.print("received ack payload is : ");
        Serial.println(rec[0]);
      }
    }
    else
    {
      Serial.println("Ack payload not available...SHIT");
    }
  }
  else {
    Serial.println("Radio.WRITE() failed");
  }
}

static bool firstTime = true;
void loop()
{
  checkControlsState();
  if (activeAction || firstTime) {
    firstTime = false;
    buildAndSendCommand(RX_FRONT_LEFT);
    delay(3);
    buildAndSendCommand(RX_FRONT_RIGHT);
    delay(3);
    buildAndSendCommand(RX_BACK_LEFT);
    delay(3);
    buildAndSendCommand(RX_BACK_RIGHT);
    delay(3);
  }
}

void buildAndSendCommand( const char receiver){
  Package package = composePackage(receiver);
  sendToReceiver(pipes[receiver], &package);
}

const Package composePackage(const char receiver)
{
  Package package;
  package |= (byte)0;

  if (variantZeroSelected)
  {
    switch (receiver)
    {
      case RX_FRONT_LEFT:
        break;
      case RX_FRONT_RIGHT:
        break;
      case RX_BACK_LEFT:
        break;
      case RX_BACK_RIGHT:
        break;
      default:
        break;
    }
  }

  if (variantOneSelected)
  {
    switch (receiver)
    {
      case RX_FRONT_LEFT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_FRONT_RIGHT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_BACK_LEFT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_BACK_RIGHT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      default:
        break;
    }
  }

  if (variantTwoSelected)
  {
    switch (receiver)
    {
      case RX_FRONT_LEFT:
        package |= 1 << LIGHT_RED_BIT;
        break;
      case RX_FRONT_RIGHT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_BACK_LEFT:
        package |= 1 << LIGHT_RED_BIT;
        break;
      case RX_BACK_RIGHT:
        package |= 1 << LIGHT_RED_BIT;
        break;
      default:
        break;
    }
  }

  if (variantThreeSelected)
  {
    switch (receiver)
    {
      case RX_FRONT_LEFT:
        package |= 1 << LIGHT_BLUE_BIT;
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_FRONT_RIGHT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_BACK_LEFT:
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      case RX_BACK_RIGHT:
        package |= 1 << LIGHT_BLUE_BIT;
        package |= 1 << LIGHT_WHITE_BIT;
        break;
      default:
        break;
    }
  }

  return package;
}

void checkControlsState()
{
  activeAction = false;
  if (digitalRead(VARIANT_ZERO_PIN)) {
    variantZeroSelected = true;
    activeAction = true;
    Serial.println("Variant ZERO selected.");
  }
  else
  {
    variantZeroSelected = false;
  }

  if (digitalRead(VARIANT_ONE_PIN)) {
    variantOneSelected = true;
    activeAction = true;
    Serial.println("Variant ONE selected.");
  }
  else
  {
    variantOneSelected = false;
  }
  
  if (digitalRead(VARIANT_TWO_PIN)) {
    variantTwoSelected = true;
    activeAction = true;
    Serial.println("Variant TWO selected.");
  }
  else
  {
    variantTwoSelected = false;
  }
  
  if (digitalRead(VARIANT_THREE_PIN)) {
    variantThreeSelected = true;
    activeAction = true;
    Serial.println("Variant THREE selected.");
  }
  else
  {
    variantThreeSelected = false;
  }
}
