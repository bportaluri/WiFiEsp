#ifndef Debug_H
#define Debug_H

#include <stdio.h>

// Change _ESPLOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings 
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#define _ESPLOGLEVEL_ 3


#define LOGERROR(x)    if(_ESPLOGLEVEL_>0) { Serial.print("[WiFiEsp] "); Serial.println(x); }
#define LOGERROR1(x,y) if(_ESPLOGLEVEL_>2) { Serial.print("[WiFiEsp] "); Serial.print(x); Serial.print(" "); Serial.println(y); }
#define LOGWARN(x)     if(_ESPLOGLEVEL_>1) { Serial.print("[WiFiEsp] "); Serial.println(x); }
#define LOGWARN1(x,y)  if(_ESPLOGLEVEL_>2) { Serial.print("[WiFiEsp] "); Serial.print(x); Serial.print(" "); Serial.println(y); }
#define LOGINFO(x)     if(_ESPLOGLEVEL_>2) { Serial.print("[WiFiEsp] "); Serial.println(x); }
#define LOGINFO1(x,y)  if(_ESPLOGLEVEL_>2) { Serial.print("[WiFiEsp] "); Serial.print(x); Serial.print(" "); Serial.println(y); }

#define LOGDEBUG(x)      if(_ESPLOGLEVEL_>3) { Serial.println(x); }
#define LOGDEBUG0(x)     if(_ESPLOGLEVEL_>3) { Serial.print(x); }
#define LOGDEBUG1(x,y)   if(_ESPLOGLEVEL_>3) { Serial.print(x); Serial.print(" "); Serial.println(y); }
#define LOGDEBUG2(x,y,z) if(_ESPLOGLEVEL_>3) { Serial.print(x); Serial.print(" "); Serial.print(y); Serial.print(" "); Serial.println(z); }


#endif
