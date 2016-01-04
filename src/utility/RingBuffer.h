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


private:

	unsigned int _size;
	char* ringBuf;
	char* ringBufEnd;
	char* ringBufP;

};

#endif