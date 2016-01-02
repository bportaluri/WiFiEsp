#include "RingBuffer.h"

#include <Arduino.h>

RingBuffer::RingBuffer(unsigned int size)
{
	_size = size;
	// add one char to terminate the string
	ringBuf = new char[size+1];
	ringBufEnd = &ringBuf[size];
	init();
}

RingBuffer::~RingBuffer() {}

void RingBuffer::reset()
{
	ringBufP = ringBuf;
}

void RingBuffer::init()
{
	ringBufP = ringBuf;
	memset(ringBuf, 0, _size+1);
}

void RingBuffer::push(char c)
{
	*ringBufP = c;
	ringBufP++;
	if (ringBufP>=ringBufEnd)
		ringBufP = ringBuf;
}



bool RingBuffer::endsWith(const char* str)
{
	int findStrLen = strlen(str);

	// b is the start position into the ring buffer
	char* b = ringBufP-findStrLen;
	if(b < ringBuf)
		b = b + _size;

	char *p1 = (char*)&str[0];
	char *p2 = p1 + findStrLen;

	for(char *p=p1; p<p2; p++)
	{
		if(*p != *b)
			return false;

		b++;
		if (b == ringBufEnd)
			b=ringBuf;
	}

	return true;
}



void RingBuffer::getStr(char * destination, unsigned int skipChars)
{
  int len = ringBufP-ringBuf-skipChars;
  
  // copy buffer to destination string
  strncpy(destination, ringBuf, len);
  
  // terminate output string
  destination[len]=0;
  
  //Serial.print("xxxxxxxxxxxxxxxxxxx");
  //Serial.println(destination);
}
