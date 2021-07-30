//
//  BuildAudioComponentDescriptionForDefaultOutput.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "BuildAudioComponentDescriptionForDefaultOutput.h"

void BuildAudioComponentDescriptionForDefaultOutput(AudioComponentDescription *oAudioComponentDescription) {
  oAudioComponentDescription->componentType = kAudioUnitType_Output;
  oAudioComponentDescription->componentSubType = kAudioUnitSubType_DefaultOutput;
  oAudioComponentDescription->componentManufacturer = kAudioUnitManufacturer_Apple;
}

