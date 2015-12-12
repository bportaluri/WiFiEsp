#ifndef RingBuffer_h
#define RingBuffer_h


class RingBuffer
{
public:
  RingBuffer(unsigned int size);
  ~RingBuffer();

 void init();
 void push(char c);
 int getPos();
 bool endsWith(const char* str);
 char * getStr(char * destination, unsigned int num);
  
private:

  unsigned int _size;
  unsigned int ringBufPos;
  char* ringBuf;

};

#endif