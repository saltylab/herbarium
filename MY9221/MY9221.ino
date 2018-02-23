#define DO 21
#define DCKO 22
#define NUM_LEDS  16

#define CMD_8BIT_MODE (0<<8)
#define CMD_12BIT_MODE (1<<8)
#define CMD_14BIT_MODE (2<<8)
#define CMD_16BIT_MODE (3<<8)

unsigned short led_bright[NUM_LEDS][3] = {0};

inline void sendCommandShort(unsigned short data){
  int dclko_state = LOW;
  for(int mask = (1<<15); mask; mask = (mask >> 1)){
    dclko_state = (dclko_state == HIGH ? LOW : HIGH);
    digitalWrite(DO, ((data & mask) ? HIGH : LOW));
    for(int i = 0; i < 10; i++)
      asm( "nop \n" );
    digitalWrite(DCKO, dclko_state);
    for(int i = 0; i < 10; i++)
      asm( "nop \n" );
  }
}

inline void sendLatch(){
  digitalWrite(DO, LOW);
  delayMicroseconds(250);
  for(int i = 0; i < 4; i++){
    digitalWrite(DO, HIGH);
    delayMicroseconds(1);
    digitalWrite(DO, LOW);
    delayMicroseconds(1);
  }
  delayMicroseconds((200 + 128*10)/1000 + 1);
}

void sendCommand(){
  for(int i = 0; i < NUM_LEDS; i++){
    // Send Flags
    sendCommandShort(CMD_16BIT_MODE);
    
    // Ignore OUTPUT_1 - OUTPUT_3
    for(int j = 0; j < 9; j++){
      sendCommandShort(0);
    }
    
    // OUTPUT_0
    for(int j = 0; j < 3; j++){
      sendCommandShort(led_bright[i][j]);
    }
  }
  // Send Latch
  sendLatch();
}

void setup() {
  pinMode(DO, OUTPUT);
  pinMode(DCKO, OUTPUT);
}

void loop() {
  for(int k = 0; k < 3; k++){
    for(long long i = 0; i < 255; i++){
      for(int j = 0; j < NUM_LEDS; j++){
        led_bright[j][0] = (k == 0? (i*i*i/256) : 0);
        led_bright[j][1] = (k == 1? (i*i*i/256) : 0);
        led_bright[j][2] = (k == 2? (i*i*i/256) : 0);
      }
      sendCommand();
    }
  }
}

