//
//  StopAudioUnit.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "StopAudioOutputUnit.h"
#include "CheckError.h"

void StopAudioOutputUnit(AudioUnit inAudioUnit) {
  CheckError(AudioOutputUnitStop(inAudioUnit), "Stopping the Output Audio Unit");
  CheckError(AudioUnitUninitialize(inAudioUnit), "Uninitializing the Output Audio Unit");
  CheckError(AudioComponentInstanceDispose(inAudioUnit), "Disposing the Output Audio Unit");
}
