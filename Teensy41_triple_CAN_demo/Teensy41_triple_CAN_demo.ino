/*
 * Demo sketch for use with:
 * https://www.skpang.co.uk/collections/teensy/products/teensy-4-1-triple-can-board-with-eth-and-u-blox-neo-m8m-gps
 *  
 * can1 baudrate 500kbps
 * 
 * can2 baudrate 500kbps
 * 
 * can3 baudrate  
 * Nominal baudrate 500kbps
 * CAN FD data baudrate 2000kbps
 * 
 * www.skpang.co.uk Dec 2021
 * 
 * 
 * */
 
#include <FlexCAN_T4.h>
FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_16> FD;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

IntervalTimer timer;

uint8_t d=0;
bool stopfd = 0;

void canSniff20(const CAN_message_t &msg) { // global callback
  Serial.print("T4: ");
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print(" OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print(" BUS "); Serial.print(msg.bus);
  Serial.print(" LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" REMOTE: "); Serial.print(msg.flags.remote);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" IDHIT: "); Serial.print(msg.idhit);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}



void setup(void) {
  delay(1000);
  Serial.begin(115200); 
  Serial.println("Teensy 4.1 Triple CAN demo. www.skpang.co.uk Dec 2021");
 
  FD.begin();

  CANFD_timings_t config;
  config.clock = CLK_24MHz;
  config.baudrate =   500000;
  config.baudrateFD = 2000000;
  config.propdelay = 190;
  config.bus_length = 1;
  config.sample = 75;
  FD.setRegions(64);
  FD.setBaudRate(config);
  FD.onReceive(canSniff);

  FD.setMBFilter(ACCEPT_ALL);
  FD.setMBFilter(MB13, 0x1);
  FD.setMBFilter(MB12, 0x1, 0x3);
  FD.setMBFilterRange(MB8, 0x1, 0x04);
  FD.enableMBInterrupt(MB8);
  FD.enableMBInterrupt(MB12);
  FD.enableMBInterrupt(MB13);
  FD.enhanceFilter(MB8);
  FD.enhanceFilter(MB10);
  FD.distribute();
  FD.mailboxStatus();

  can2.begin();
  can2.setBaudRate(500000);
  can2.setMBFilter(ACCEPT_ALL);
  can2.distribute();
  can2.mailboxStatus();
  
  can1.begin();
  can1.setBaudRate(500000);
  can1.setMBFilter(ACCEPT_ALL);
  can1.distribute();
  can1.mailboxStatus();

 
  timer.begin(sendframe, 500000); // Send frame every 500ms
}

void sendframe()
{
  CAN_message_t msg2;
  msg2.id = 0x40;
  
  for ( uint8_t i = 0; i < 8; i++ ) {
    msg2.buf[i] = i + 1;
  }
  
  msg2.buf[0] = d++;
  msg2.seq = 1;
  can2.write(MB15, msg2);
 
  CAN_message_t msg1;
  msg1.id = 0x7df;
  msg1.buf[0] = d;
  can1.write(msg1);

  
  CANFD_message_t msg;
  msg.len = 64;
  msg.id = 0x321;
  msg.seq = 1;
  msg.buf[0] = d; msg.buf[1] = 2; msg.buf[2] = 3; msg.buf[3] = 4;
  msg.buf[4] = 5; msg.buf[5] = 6; msg.buf[6] = 9; msg.buf[7] = 9;
  FD.write( msg);
}

void loop() {
  
  FD.events(); /* needed for sequential frame transmit and callback queues */
  CANFD_message_t msg;
  CAN_message_t msgcan1;
  CAN_message_t msgcan2;
  
  if(FD.readMB(msg)){   /* check if we received a CAN frame */
      Serial.print("MB: "); Serial.print(msg.mb);
      Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
      Serial.print("  ID: 0x"); Serial.print(msg.id, HEX );
      Serial.print("  EXT: "); Serial.print(msg.flags.extended );
      Serial.print("  LEN: "); Serial.print(msg.len);
      Serial.print("  BRS: "); Serial.print(msg.brs);
      Serial.print(" DATA: ");
      for ( uint8_t i = 0; i <msg.len ; i++ ) {
        Serial.print(msg.buf[i]); Serial.print(" ");
      }
      Serial.print("  TS: "); Serial.println(msg.timestamp);
    
  }

  if(can1.read(msgcan1))
  {   /* check if we received a CAN frame */
      Serial.println("Got can1");
      Serial.print("MB: "); Serial.print(msgcan1.mb);
      Serial.print("  OVERRUN: "); Serial.print(msgcan1.flags.overrun);
      Serial.print("  ID: 0x"); Serial.print(msgcan1.id, HEX );
      Serial.print("  EXT: "); Serial.print(msgcan1.flags.extended );
      Serial.print("  LEN: "); Serial.print(msgcan1.len);
      
      Serial.print(" DATA: ");
      for ( uint8_t i = 0; i <msgcan1.len ; i++ ) {
        Serial.print(msgcan1.buf[i]); Serial.print(" ");
      }
      Serial.print("  TS: "); Serial.println(msgcan1.timestamp);
    
  }

  if(can2.read(msgcan2))
  {   /* check if we received a CAN frame */
      Serial.println("Got can2");
      Serial.print("MB: "); Serial.print(msgcan2.mb);
      Serial.print("  OVERRUN: "); Serial.print(msgcan2.flags.overrun);
      Serial.print("  ID: 0x"); Serial.print(msgcan2.id, HEX );
      Serial.print("  EXT: "); Serial.print(msgcan2.flags.extended );
      Serial.print("  LEN: "); Serial.print(msgcan2.len);
      
      Serial.print(" DATA: ");
      for ( uint8_t i = 0; i <msgcan2.len ; i++ ) {
        Serial.print(msgcan2.buf[i]); Serial.print(" ");
      }
      Serial.print("  TS: "); Serial.println(msgcan2.timestamp);
    
  }
 
}

void canSniff(const CANFD_message_t &msg) {
  if ( stopfd ) return;
  Serial.print("ISR - MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}
