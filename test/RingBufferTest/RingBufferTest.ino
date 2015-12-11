
#include "WiFiEsp.h"

const char* TAGS[] =
{
  "123",
  "000"
};


RingBuffer buf(10);

void setup()
{
  Serial.begin(115200);
  
  buf.init();

  buf.push('a');
  Serial.println(buf.endsWith("a"));
  Serial.println(buf.endsWith("A"));
  Serial.println(buf.endsWith("ab"));
  Serial.println();
  
  buf.push('b');
  Serial.println(buf.endsWith("a"));
  Serial.println(buf.endsWith("A"));
  Serial.println(buf.endsWith("ab"));
  Serial.println();

  buf.push('b');
  Serial.println(buf.endsWith("a"));
  Serial.println(buf.endsWith("A"));
  Serial.println(buf.endsWith("ab"));
  Serial.println();
}


void loop()
{
}
