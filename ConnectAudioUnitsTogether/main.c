//
//  main.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 28/7/21.
//

#include <stdio.h>
#include <AudioToolbox/AudioToolbox.h>
#include "CheckError.h"
#include "FindAudioComponent.h"
#include "BuildAudioComponentDescriptionForFilePlayer.h"
#include "BuildAudioComponentDescriptionForDefaultOutput.h"
#include "OpenAudioFile.h"
#include "PlayerData.h"
#include "GetAudioFileAudioStreamBasicDescription.h"
#include "GetNumberOfPackets.h"
#include "StopAudioOutputUnit.h"
#include "StopFilePlayerAudioUnit.h"
#include "CloseAudioFile.h"

AudioUnit SetUpFilePlayerUnit(PlayerData *playerData) {
  AudioComponentDescription filePlayerAudioComponentDescription = {0};
  
  BuildAudioComponentDescriptionForFilePlayer(&filePlayerAudioComponentDescription);
    
  AudioComponent filePlayerAudioComponent = FindAudioComponent(filePlayerAudioComponentDescription);
    
  AudioUnit filePlayerAudioUnit;
    
  CheckError(AudioComponentInstanceNew(filePlayerAudioComponent,
                                       &filePlayerAudioUnit),
             "Instantiating the File Player Audio Unit");
  
  CheckError(AudioUnitInitialize(filePlayerAudioUnit), "Initializing the filePlayerAudioUnit");
  
  CheckError(AudioUnitSetProperty(filePlayerAudioUnit,
                                  kAudioUnitProperty_ScheduledFileIDs,
                                  kAudioUnitScope_Global,
                                  0,
                                  &(playerData->inputFile),
                                  sizeof(playerData->inputFile)),
             "Audio Unit setting property value for kAudioUnitProperty_ScheduledFileIDs");
  
  UInt64 packetsCount = 0;
  GetNumberOfPackets(playerData->inputFile, "Getting the number of packets from the input file", &packetsCount);
  
  // Tell the Audio Unit for file player to play the whole file
  ScheduledAudioFileRegion rgn;
  
  memset(&(rgn.mTimeStamp), 0, sizeof(rgn.mTimeStamp));
  rgn.mTimeStamp.mFlags = kAudioTimeStampSampleTimeValid;
  rgn.mTimeStamp.mSampleTime = 0;
  
  rgn.mCompletionProc = NULL;
  rgn.mCompletionProcUserData = NULL;
  rgn.mAudioFile = playerData->inputFile;
  rgn.mLoopCount = 0;
  rgn.mStartFrame = 0;
  rgn.mFramesToPlay = packetsCount * playerData->inputFormat.mFramesPerPacket;
  
  CheckError(AudioUnitSetProperty(filePlayerAudioUnit,
                                  kAudioUnitProperty_ScheduledFileRegion,
                                  kAudioUnitScope_Global,
                                  0,
                                  &rgn,
                                  sizeof(rgn)),
             "Setting the audio unit property kAudioUnitProperty_ScheduledFileRegion");

  // Tell when to start playing back
  AudioTimeStamp startTime;
  memset(&startTime, 0, sizeof(startTime));
  startTime.mFlags = kAudioTimeStampSampleTimeValid;
  startTime.mSampleTime = -1;

  CheckError(AudioUnitSetProperty(filePlayerAudioUnit,
                                  kAudioUnitProperty_ScheduleStartTimeStamp,
                                  kAudioUnitScope_Global,
                                  0,
                                  &startTime,
                                  sizeof(startTime)),
             "Setting the desired start for the play back to be as soon as possible");
  
  return filePlayerAudioUnit;
}

AudioUnit SetUpDefaultOutputAudioUnit() {
  AudioComponentDescription outputDefaultDescription = {0};
  
  BuildAudioComponentDescriptionForDefaultOutput(&outputDefaultDescription);
    
  AudioComponent defaultOutputAudioComponent = FindAudioComponent(outputDefaultDescription);
    
  AudioUnit defaultOutputAudioUnit;
    
  CheckError(AudioComponentInstanceNew(defaultOutputAudioComponent,
                                       &defaultOutputAudioUnit),
             "Instantiating the Default Output Audio Unit");

  CheckError(AudioUnitInitialize(defaultOutputAudioUnit), "Initializing the defaultOutputAudioUnit");

  return defaultOutputAudioUnit;
}

void ConnectUnitsTogether(AudioUnit filePlayerAudioUnit, AudioUnit defaultOutputAudioUnit) {
  AudioUnitConnection connection;
  connection.destInputNumber = 0;
  connection.sourceAudioUnit = filePlayerAudioUnit;
  connection.sourceOutputNumber = 0;
  
  CheckError(AudioUnitSetProperty(defaultOutputAudioUnit,
                                  kAudioUnitProperty_MakeConnection,
                                  kAudioUnitScope_Input,
                                  0,
                                  &connection,
                                  sizeof(AudioUnitConnection)),
             "connecting filer player to default output unit");
}

void StartDefaultOutputUnit(AudioUnit defaultOutputAudioUnit) {
  // need to start the default output unit
  CheckError(AudioOutputUnitStart(defaultOutputAudioUnit),
             "Starting the Output Audio Unit");
}

int main(int argc, const char * argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "1st argument: You need to give the input file for playing back. You can use any Core Audio supported file such as .mp3, .aac, .m4a, .wav, .aif e.t.c.\n");
    return 1;
  }
  
  PlayerData playerData = {0};
  
  OpenAudioFile(argv[1], &playerData.inputFile);
  
  GetAudioFileAudioStreamBasicDescription(playerData.inputFile, &playerData.inputFormat);
  
  AudioUnit filePlayerAudioUnit = SetUpFilePlayerUnit(&playerData);

  AudioUnit defaultOutputAudioUnit = SetUpDefaultOutputAudioUnit();
  
  ConnectUnitsTogether(filePlayerAudioUnit, defaultOutputAudioUnit);
    
  StartDefaultOutputUnit(defaultOutputAudioUnit);
  
  printf("Sleeping....\n");
  
  sleep(54);
  
  // need to release resources
  StopAudioOutputUnit(defaultOutputAudioUnit);
  StopFilePlayerAudioUnit(filePlayerAudioUnit);
  CloseAudioFile(playerData.inputFile);
    
  return 0;
}
