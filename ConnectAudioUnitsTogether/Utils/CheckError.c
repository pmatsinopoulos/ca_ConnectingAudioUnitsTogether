//
//  CheckError.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <MacTypes.h>
#include <CoreFoundation/CoreFoundation.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "CheckError.h"

void CheckError(OSStatus error, const char *operation) {
  if (error == noErr) {
    return;
  }
  
  char errorString[20];
  *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error); // we have 4 bytes and we put them in Big-endian ordering. 1st byte the biggest
  if (isprint(errorString[1]) && isprint(errorString[2]) &&
      isprint(errorString[3]) && isprint(errorString[4])) {
    errorString[0] = errorString[5] = '\'';
    errorString[6] = '\0';
  } else {
    sprintf(errorString, "%d", (int) error);
  }
  fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
  exit(1);
}
