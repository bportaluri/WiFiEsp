
#define _DEBUG_ 1

#ifndef Debug_H
#define Debug_H

#include <stdio.h>
#include <string.h>

#define PRINT_FILE_LINE() do { 						\
		Serial.print("[");Serial.print(__FILE__);		\
		Serial.print("::");Serial.print(__LINE__);Serial.print("]");\
}while (0);

#ifdef _DEBUG_



#define INFO1(x) do { Serial.println(x);    			\
}while (0);

#define INFO2(x,y) do { Serial.print(x); Serial.print(" "); Serial.println(y);    			\
}while (0);

#define INFO3(x,y,z) do { Serial.print(x); Serial.print(" "); Serial.print(y); Serial.print(" "); Serial.println(z);    			\
}while (0);


#else
#define INFO1(x) do {} while(0);
#define INFO2(x,y) do {} while(0);
#define INFO3(x,y,z) do {} while(0);
#define INFO(format, args...) do {} while(0);
#endif

#if 0
#define WARN(args) do { PRINT_FILE_LINE()			\
		Serial.print("-W-"); Serial.println(args);	\
}while (0);
#else
#define WARN(args) do {} while (0);
#endif


#endif
