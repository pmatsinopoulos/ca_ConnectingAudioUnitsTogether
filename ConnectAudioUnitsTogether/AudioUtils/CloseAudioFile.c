//
//  CloseAudioFile.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "CloseAudioFile.h"
#include "CheckError.h"

void CloseAudioFile(AudioFileID inFileHandle) {
  CheckError(AudioFileClose(inFileHandle), "Close File");
}
