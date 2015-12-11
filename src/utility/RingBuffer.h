#ifndef RingBuffer_h
#define RingBuffer_h



class RingBuffer
{
public:
	RingBuffer(unsigned int size);
  ~RingBuffer();

	void init();
	void push(char c);
	
	bool endsWith(const char* str);
  
private:

  unsigned int _size;
  unsigned int ringBufPos;
	char* ringBuf;


};

#endif
