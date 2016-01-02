/*
 WiFiEsp test: RingBufferTest
 
 Test of the RingBuffer class.
*/

#include "WiFiEsp.h"

RingBuffer buf(5);


void setup()
{
  Serial.begin(115200);
  
  buf.init();

  buf.push('a');
  Serial.println(buf.ringBuf);
  assert(10, buf.endsWith("a"), true);
  assert(11, buf.endsWith("A"), false);
  assert(12, buf.endsWith("ab"), false);

  buf.push('b');
  Serial.println(buf.ringBuf);
  assert(21, buf.endsWith("a"), false);
  assert(22, buf.endsWith("A"), false);
  assert(23, buf.endsWith("ab"), true);

  buf.push('c');
  buf.push('d');
  buf.push('e');
  Serial.println(buf.ringBuf);
  assert(31, buf.endsWith("abcde"), true);
  assert(32, buf.endsWith("de"), true);

  buf.push('f');
  Serial.println(buf.ringBuf);
  assert(43, buf.endsWith("bcdef"), true);
  assert(44, buf.endsWith("ef"), true);

/*
  Serial.println("Speed test");
  
  buf.init();
  
  buf.push('a');
  buf.push('b');
  buf.push('c');
  buf.push('d');
  buf.push('e');
  

  int t0;

  t0 = micros();
  for(unsigned int i=0; i<100; i++)
    buf.endsWith("bcde");
  Serial.println(micros()-t0);

  t0 = micros();
  for(unsigned int i=0; i<100; i++)
    buf.endsWith2("bcde");
  Serial.println(micros()-t0);

  t0 = micros();
  for(unsigned int i=0; i<100; i++)
    buf.push('x');
  Serial.println(micros()-t0);

  t0 = micros();
  for(unsigned int i=0; i<100; i++)
    buf.push2('x');
  Serial.println(micros()-t0);
*/
}


void loop()
{
}


void assert(int i, bool x, bool y)
{
  if (x!=y)
  {
    Serial.print ("FAIL ");
    Serial.println(i);
  }
}

