//
//  PlayerData.h
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#ifndef PlayerData_h
#define PlayerData_h

#include <CoreAudio/CoreAudio.h>

typedef struct PlayerData {
  AudioStreamBasicDescription inputFormat;
  AudioFileID                 inputFile;
  Float64                     fileDurationInSeconds;
} PlayerData;


#endif /* PlayerData_h */
