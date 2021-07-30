//
//  BuildAudioComponentDescriptionForFilePlayer.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "BuildAudioComponentDescriptionForFilePlayer.h"

void BuildAudioComponentDescriptionForFilePlayer(AudioComponentDescription *oAudioComponentDescription) {
  oAudioComponentDescription->componentType = kAudioUnitType_Generator;
  oAudioComponentDescription->componentSubType = kAudioUnitSubType_AudioFilePlayer;
  oAudioComponentDescription->componentManufacturer = kAudioUnitManufacturer_Apple;
}
