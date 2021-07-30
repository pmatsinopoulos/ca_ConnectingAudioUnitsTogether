//
//  GetAudioFileAudioStreamBasicDescription.h
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#ifndef GetAudioFileAudioStreamBasicDescription_h
#define GetAudioFileAudioStreamBasicDescription_h

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>

void GetAudioFileAudioStreamBasicDescription(AudioFileID iAudioFileHandler,
                                             AudioStreamBasicDescription *oAudioFormat);

#endif /* GetAudioFileAudioStreamBasicDescription_h */
