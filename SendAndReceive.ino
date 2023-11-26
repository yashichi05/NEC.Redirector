#include <Arduino.h>
#define SEND_REPEAT 3

void setup() {
  Serial.begin(9600);
}

const char emptyCmd[3] = { 0, 0, 0 };

struct cmd {
  const char shutdown[3];
  const char volUp[3];
  const char volDown[3];
  const char source[3];
};
cmd sourceCmd = {
  { 0x7, 0x7, 0x2 },
  { 0x7, 0x7, 0x7 },
  { 0x7, 0x7, 0xB },
  { 0x7, 0x7, 0x1 }
};
cmd targetCmd = {
  { 0x9, 0xF6, 0x45 },
  { 0x9, 0xF6, 0x48 },
  { 0x9, 0xF6, 0x1C },
  { 0x9, 0xF6, 0x10 }
};

char gotNEC[3] = { 0, 0, 0 };


void readNEC(int index = 0) {
  if (Serial.available() <= 0) return;
  gotNEC[index] = Serial.read();
  if (index == 2) {
    printNEC();
    return;
  }
  readNEC(index + 1);
}

void printNEC() {
  for (int i = 0; i < sizeof(gotNEC); ++i) {
    Serial.print(gotNEC[i], HEX);
    Serial.print(",");
  }
  Serial.print("\n");
}

void transformNEC() {
  if (isEqual(gotNEC, emptyCmd) == 0) {
    return;
  } else if (isEqual(gotNEC, sourceCmd.shutdown) == 0) {
    sendNEC(targetCmd.shutdown);
    sendNEC(targetCmd.shutdown);
  } else if (isEqual(gotNEC, sourceCmd.volUp) == 0) {
    sendNEC(targetCmd.volUp);
  } else if (isEqual(gotNEC, sourceCmd.volDown) == 0) {
    sendNEC(targetCmd.volDown);
  } else if (isEqual(gotNEC, sourceCmd.source) == 0) {
    sendNEC(targetCmd.source);
  }
  memset(gotNEC, 0, 3);
}

int isEqual(const char a[3], const char b[3]) {
  for (int i = 0; i < 3; ++i) {
    if (a[i] != b[i]) return (i + 1);
  }
  return 0;
}

void sendNEC(const char nec[3]) {
  char command[5] = { 0xA1, 0xF1, nec[0], nec[1], nec[2] };
  for (int i = 0; i < SEND_REPEAT; ++i) {
    Serial.write(command, 5);
    delay(200);
  }
}

void loop() {
  Serial.flush();
  readNEC();
  transformNEC();
  delay(100);  // Loop delay
}
