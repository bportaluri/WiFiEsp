/*--------------------------------------------------------------------
This file is part of the Arduino WiFiEsp library.

The Arduino WiFiEsp library is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The Arduino WiFiEsp library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Arduino WiFiEsp library.  If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#ifndef RingBuffer_h
#define RingBuffer_h


class RingBuffer
{
public:
	RingBuffer(unsigned int size);
	~RingBuffer();

	void reset();
	void init();
	void push(char c);
	int getPos();
	bool endsWith(const char* str);
	void getStr(char * destination, unsigned int skipChars);
	void getStrN(char * destination, unsigned int skipChars, unsigned int num);


private:

	unsigned int _size;
	char* ringBuf;
	char* ringBufEnd;
	char* ringBufP;

};

#endif