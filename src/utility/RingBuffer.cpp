#include "RingBuffer.h"

#include <Arduino.h>

RingBuffer::RingBuffer(unsigned int size)
{
  _size = size;
  ringBuf = new char[size];
}

RingBuffer::~RingBuffer() {}

void RingBuffer::init()
{
  ringBufPos = 0;
}

void RingBuffer::push(char c)
{
  ringBuf[ringBufPos % _size] = c;
  ringBufPos++;
}

int RingBuffer::getPos()
{
  return ringBufPos;
}


bool RingBuffer::endsWith(const char* str)
{
  int findStrLen = strlen(str);
  
  if(ringBufPos < findStrLen)
    return false;

  unsigned int j = ringBufPos-findStrLen;

  for(unsigned int i=0; i<findStrLen; i++)
  {
    if(str[i] != ringBuf[(j+i) % _size])
        return false;
  }
  
  return true;
}


char * RingBuffer::getStr(char * destination, unsigned int num)
{
  // copy buffer to destination string
  char * ret = strncpy(destination, ringBuf, num);
  
  // terminate output string
  destination[num]=0;
  
  return ret;
}
