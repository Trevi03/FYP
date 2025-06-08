#include "ADS129X.h"
#include <SPI.h>
#include "mbed.h"

#define DEBUG_MODE true
#if DEBUG_MODE
  #define DEBUG_PRINT(x)  Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

/* ADS129X pins */
const int ADS_PWDN = 10;
const int ADS_RESET  = 9;
const int ADS_START  = 8;
const int ADS_DRDY = 2;
const int ADS_CS = 7;
const int ADS_CLK = A1;

int fourbfiveb[] = {
	0b11110, // Hex data 0
	0b01001, // Hex data 1
	0b10100, // Hex data 2
	0b10101, // Hex data 3
	0b01010, // Hex data 4
	0b01011, // Hex data 5
	0b01110, // Hex data 6
	0b01111, // Hex data 7
	0b10010, // Hex data 8
	0b10011, // Hex data 9
	0b10110, // Hex data A
	0b10111, // Hex data B
	0b11010, // Hex data C
	0b11011, // Hex data D
	0b11100, // Hex data E
	0b11101  // Hex data F
};

#define encode4B5B(x) (fourbfiveb[x&0x0F])
#define HIGH_NIBBLE(x) (x>>4)
#define LOW_NIBBLE(x) (x&0x0F)
#define LED LEDG
// #define ADS129X_POLLING

/*uint32_t fourbfiveb[256] = {
  0x1e1e, 0x1e09, 0x1e14, 0x1e15, 0x1e0a, 0x1e0b, 0x1e0e, 0x1e0f,
  0x1e12, 0x1e13, 0x1e16, 0x1e17, 0x1e1a, 0x1e1b, 0x1e1c, 0x1e1d,
  0x091e, 0x0909, 0x0914, 0x0915, 0x090a, 0x090b, 0x090e, 0x090f,
  0x0912, 0x0913, 0x0916, 0x0917, 0x091a, 0x091b, 0x091c, 0x091d,
  0x141e, 0x1409, 0x1414, 0x1415, 0x140a, 0x140b, 0x140e, 0x140f,
  0x1412, 0x1413, 0x1416, 0x1417, 0x141a, 0x141b, 0x141c, 0x141d,
  0x151e, 0x1509, 0x1514, 0x1515, 0x150a, 0x150b, 0x150e, 0x150f,
  0x1512, 0x1513, 0x1516, 0x1517, 0x151a, 0x151b, 0x151c, 0x151d,
  0x0a1e, 0x0a09, 0x0a14, 0x0a15, 0x0a0a, 0x0a0b, 0x0a0e, 0x0a0f,
  0x0a12, 0x0a13, 0x0a16, 0x0a17, 0x0a1a, 0x0a1b, 0x0a1c, 0x0a1d,
  0x0b1e, 0x0b09, 0x0b14, 0x0b15, 0x0b0a, 0x0b0b, 0x0b0e, 0x0b0f,
  0x0b12, 0x0b13, 0x0b16, 0x0b17, 0x0b1a, 0x0b1b, 0x0b1c, 0x0b1d,
  0x0e1e, 0x0e09, 0x0e14, 0x0e15, 0x0e0a, 0x0e0b, 0x0e0e, 0x0e0f,
  0x0e12, 0x0e13, 0x0e16, 0x0e17, 0x0e1a, 0x0e1b, 0x0e1c, 0x0e1d,
  0x0f1e, 0x0f09, 0x0f14, 0x0f15, 0x0f0a, 0x0f0b, 0x0f0e, 0x0f0f,
  0x0f12, 0x0f13, 0x0f16, 0x0f17, 0x0f1a, 0x0f1b, 0x0f1c, 0x0f1d,
  0x121e, 0x1209, 0x1214, 0x1215, 0x120a, 0x120b, 0x120e, 0x120f,
  0x1212, 0x1213, 0x1216, 0x1217, 0x121a, 0x121b, 0x121c, 0x121d,
  0x131e, 0x1309, 0x1314, 0x1315, 0x130a, 0x130b, 0x130e, 0x130f,
  0x1312, 0x1313, 0x1316, 0x1317, 0x131a, 0x131b, 0x131c, 0x131d,
  0x161e, 0x1609, 0x1614, 0x1615, 0x160a, 0x160b, 0x160e, 0x160f,
  0x1612, 0x1613, 0x1616, 0x1617, 0x161a, 0x161b, 0x161c, 0x161d,
  0x171e, 0x1709, 0x1714, 0x1715, 0x170a, 0x170b, 0x170e, 0x170f,
  0x1712, 0x1713, 0x1716, 0x1717, 0x171a, 0x171b, 0x171c, 0x171d,
  0x1a1e, 0x1a09, 0x1a14, 0x1a15, 0x1a0a, 0x1a0b, 0x1a0e, 0x1a0f,
  0x1a12, 0x1a13, 0x1a16, 0x1a17, 0x1a1a, 0x1a1b, 0x1a1c, 0x1a1d,
  0x1b1e, 0x1b09, 0x1b14, 0x1b15, 0x1b0a, 0x1b0b, 0x1b0e, 0x1b0f,
  0x1b12, 0x1b13, 0x1b16, 0x1b17, 0x1b1a, 0x1b1b, 0x1b1c, 0x1b1d,
  0x1c1e, 0x1c09, 0x1c14, 0x1c15, 0x1c0a, 0x1c0b, 0x1c0e, 0x1c0f,
  0x1c12, 0x1c13, 0x1c16, 0x1c17, 0x1c1a, 0x1c1b, 0x1c1c, 0x1c1d,
  0x1d1e, 0x1d09, 0x1d14, 0x1d15, 0x1d0a, 0x1d0b, 0x1d0e, 0x1d0f,
  0x1d12, 0x1d13, 0x1d16, 0x1d17, 0x1d1a, 0x1d1b, 0x1d1c, 0x1d1d
};*/

// #define encode4B5B(x) (fourbfiveb[x&0xFF])

ADS129X ADS = ADS129X(ADS_DRDY, ADS_CS);

// For ADS external clk
#define SAMPLES_PER_SECOND  (4000000)
#define PPI_CHANNEL_T4      (7)

// A1 on Nano 33 BLE → P0.05
#define PIN_GPIO_T4         (5)
#define PORT_GPIO_T4        (0)

void initTimer4(){
  // Generates a periodic event using CC[0] (compare register)
  // For a 1 MHz square wave, you need 2 million transitions per second
  NRF_TIMER4->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER4->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
  NRF_TIMER4->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
  NRF_TIMER4->PRESCALER = 0;
  NRF_TIMER4->CC[0] = 16000000 / SAMPLES_PER_SECOND; // Needs prescaler set to 0 (1:1) 16MHz clock
  NRF_TIMER4->TASKS_START = 1;
}

void initGPIOTE(){
  // Configured to toggle a GPIO pin (here, P0.05 = A1) whenever triggered
  NRF_GPIOTE->CONFIG[0] = ( GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos ) |
                          ( GPIOTE_CONFIG_OUTINIT_Low     << GPIOTE_CONFIG_OUTINIT_Pos ) |
                          ( GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos ) |
                          ( PORT_GPIO_T4                  << GPIOTE_CONFIG_PORT_Pos ) |
                          ( PIN_GPIO_T4                   << GPIOTE_CONFIG_PSEL_Pos );
}
void initPPI(){
  // Configure PPI channel with connection between TIMER->EVENTS_COMPARE[0] and GPIOTE->TASKS_OUT[0]
  // Connects the TIMER4 compare event to the GPIOTE toggle task
  NRF_PPI->CH[PPI_CHANNEL_T4].EEP = ( uint32_t )&NRF_TIMER4->EVENTS_COMPARE[0];
  NRF_PPI->CH[PPI_CHANNEL_T4].TEP = ( uint32_t )&NRF_GPIOTE->TASKS_OUT[0];

  // Enable PPI channel
  NRF_PPI->CHENSET = ( 1UL << PPI_CHANNEL_T4 );
}


void setup() {
  // Initialise CLK
  initTimer4();
  initGPIOTE();
  initPPI();

  if (DEBUG_MODE) {
    Serial.begin(9600);
    while (!Serial); // wait for serial monitor to open
  }
  DEBUG_PRINTLN("Start Setup");

  pinMode(ADS_PWDN, OUTPUT);
  pinMode(ADS_START, OUTPUT);
  pinMode(ADS_RESET, OUTPUT);
  // pinMode(LED,OUTPUT);
  pinMode(ADS_CLK, OUTPUT);

  digitalWrite(ADS_START, LOW);
  digitalWrite(ADS_PWDN, HIGH); // turn off power down mode
  // digitalWrite(LED, LOW);
  delay(500); // delay for power-on-reset (Datasheet, pg. 48)

  // reset pulse
  digitalWrite(ADS_RESET, LOW);
  digitalWrite(ADS_RESET, HIGH);
  delay(50); // Wait for 18 tCLKs AKA 9 microseconds, we use 1 millisec


  ADS.BEGIN(); 
  ADS.SDATAC(); // device wakes up in RDATAC mode, so send stop signal

  ADS.WREG(ADS129X_REG_CONFIG1, ADS129X_SAMPLERATE_1024); // enable 8kHz sample-rate
  ADS.WREG(ADS129X_REG_CONFIG3, (1<<ADS129X_BIT_PD_REFBUF) | (1<<6)); // enable internal reference
  //ADS.WREG(ADS129X_REG_CONFIG2, (1<<ADS129X_BIT_INT_TEST) | ADS129X_TEST_FREQ_2HZ);
  
  // setup channels
  ADS.configChannel(1, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(2, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(3, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(4, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(5, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(6, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(7, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);
  ADS.configChannel(8, false, ADS129X_GAIN_1X, ADS129X_MUX_NORMAL);

  delay(1);
  ADS.RDATAC();
  ADS.START();

  DEBUG_PRINTLN("COmplete Setup");
}

// void loop() {
//   int drdyState = digitalRead(ADS_DRDY);  // Read the pin state
  
//   if (drdyState == LOW) {
//     DEBUG_PRINTLN("DRDY is LOW - Data Ready");
//   } else {
//     DEBUG_PRINTLN("DRDY is HIGH - Data Not Ready");
//   }

//   // delay(10); // Polling interval (adjust as needed)
// }

void loop() {
  long buffer[9];
  // static unsigned long tLast;
  // if (millis()-tLast > 500) {
  //   digitalWrite(LED, !digitalRead(LED));
  //   tLast = millis();
  // }
  if (ADS.getData(buffer)) {
    // digitalWrite(LED, !digitalRead(LED));
    DEBUG_PRINTLN("Toggle LED");
    for (int channel = 1; channel < 9; channel++) {
      long value = buffer[channel];

      // // Debug: print readable value
      // DEBUG_PRINT("CH");
      // DEBUG_PRINT(channel);
      // DEBUG_PRINT(": ");
      // DEBUG_PRINTLN(value);

      // // Then send over BLE using Serial.write() (or a BLE characteristic)
      // byte valueBytes[3];
      // byte encoded[7];
      // byte packet[5];

      // valueBytes[0] = (byte)(value >> 16);
      // valueBytes[1] = (byte)(value >> 8);
      // valueBytes[2] = (byte)(value);

      // encoded[0] = encode4B5B(LOW_NIBBLE(channel));
      // encoded[1] = encode4B5B(HIGH_NIBBLE(valueBytes[0]));
      // encoded[2] = encode4B5B(LOW_NIBBLE(valueBytes[0]));
      // encoded[3] = encode4B5B(HIGH_NIBBLE(valueBytes[1]));
      // encoded[4] = encode4B5B(LOW_NIBBLE(valueBytes[1]));
      // encoded[5] = encode4B5B(HIGH_NIBBLE(valueBytes[2]));
      // encoded[6] = encode4B5B(LOW_NIBBLE(valueBytes[2]));

      // packet[0] = 0 | (encoded[0] >> 2);
      // packet[1] = (encoded[0] << 6) | (encoded[1] << 1) | (encoded[2] >> 4);
      // packet[2] = (encoded[2] << 4) | (encoded[3] >> 1);
      // packet[3] = (encoded[3] << 7) | (encoded[4] << 2) | (encoded[5] >> 3);
      // packet[4] = (encoded[5] << 5) | (encoded[6]);

      // Serial.write(packet, 5); // or send via BLE characteristic
    }
  }
}
