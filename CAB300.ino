// CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
byte rxBuf[8];
char msgString[128];                        // Array to store serial string
int debug = 0;
uint32_t inbox;
signed long milliamps;

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(9);                               // Set CS to pin 9 de[ending on shield used


void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input

  Serial.println("Time Stamp,ID,Extended,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8");
}

void loop()
{
  if (CAN0.checkReceive() == 3)                        // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)

    switch (rxId)
    {
      case 0x3c2:
      CAB300(rxBuf);
        break;

      default:
        break;
    }
    if (debug == 1)
    {
      Serial.print(millis());
      if ((rxId & 0x80000000) == 0x80000000)    // Determine if ID is standard (11 bits) or extended (29 bits)
        sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
      else
        sprintf(msgString, ",0x%.3lX,false,%1d", rxId, len);

      Serial.print(msgString);

      if ((rxId & 0x40000000) == 0x40000000) {  // Determine if message is a remote request frame.
        sprintf(msgString, " REMOTE REQUEST FRAME");
        Serial.print(msgString);
      } else {
        for (byte i = 0; i < len; i++) {
          sprintf(msgString, ", 0x%.2X", rxBuf[i]);
          Serial.print(msgString);
        }
      }

      Serial.println();
    }
  }
}

void CAB300(byte data[8])
{
  for (int i = 0; i<4; i++)
  {
    inbox = (inbox <<8) | data[i];
  }
  milliamps = inbox;
  if (milliamps > 0x80000000)
  {
    milliamps -= 0x80000000;
  }
  else {
    milliamps = (0x80000000 - milliamps) * -1;
  }
  Serial.println();
  Serial.print(milliamps);
  Serial.print("mA");
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
