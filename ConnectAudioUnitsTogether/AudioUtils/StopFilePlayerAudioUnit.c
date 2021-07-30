//
//  StopFilePlayerAudioUnit.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "StopFilePlayerAudioUnit.h"
#include "CheckError.h"

void StopFilePlayerAudioUnit(AudioUnit filePlayerAudioUnit) {
  CheckError(AudioUnitUninitialize(filePlayerAudioUnit), "Uninitializing the FilePlayer Audio Unit");
  CheckError(AudioComponentInstanceDispose(filePlayerAudioUnit), "Disposing the FilePlayer Audio Unit");
}
