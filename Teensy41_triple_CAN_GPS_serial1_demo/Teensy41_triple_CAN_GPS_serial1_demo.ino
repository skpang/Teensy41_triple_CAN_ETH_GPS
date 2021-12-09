/*

Demo sketch for use with:
https://www.skpang.co.uk/collections/teensy/products/teensy-4-1-triple-can-board-with-eth-and-u-blox-neo-m8m-gps

GPS demo using Serial2

*/

void pps() {
  Serial.println("PPS");
}

void setup() {
  
  delay(1000);
  Serial.begin(115200);
  Serial.println("Teensy 4.1 Triple CAN GPS demo using Serial2. www.skpang.co.uk Dec 2021");
  
  attachInterrupt(35, pps, RISING);  // PPS on D35

  Serial.println("Changing baudrate");
  Serial2.begin(9600);
  while (Serial2.available()) Serial2.read();
  Serial2.write("$PUBX,41,1,0003,0003,115200,0*1C\n\r");
  delay(1100);

  Serial.println("New baudrate 115.2kbps");
  Serial2.begin(115200);
  while (Serial2.available()) Serial2.read();
  
}

void loop() {

  while (Serial2.available()) {
    char c = Serial2.read();
    Serial.print(c);
  }
}
