/*
 WiFiEsp test: RingBufferTest
 
 Test of the RingBuffer class.
*/

#include "WiFiEsp.h"

RingBuffer buf(5);


void setup()
{
  Serial.begin(115200);
  
  Serial.println("Starting tests");

  buf.init();

  buf.push('a');
  assert(10, buf.endsWith("a"), true);
  assert(11, buf.endsWith("A"), false);
  assert(12, buf.endsWith("ab"), false);

  buf.push('b');
  assert(21, buf.endsWith("a"), false);
  assert(22, buf.endsWith("A"), false);
  assert(23, buf.endsWith("ab"), true);

  buf.push('c');
  buf.push('d');
  buf.push('e');
  assert(31, buf.endsWith("abcde"), true);
  assert(32, buf.endsWith("de"), true);

  buf.push('f');
  assert(43, buf.endsWith("bcdef"), true);
  assert(44, buf.endsWith("ef"), true);

  Serial.println("Done");
}


void loop()
{
  // nothing to do
}


void assert(int i, bool x, bool y)
{
  if (x!=y)
  {
    Serial.print ("FAIL ");
    Serial.println(i);
  }
}

