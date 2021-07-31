//
//  main.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 28/7/21.
//

#include <stdio.h>
#include <AudioToolbox/AudioToolbox.h>
#include <pthread/pthread.h>
#include "CheckError.h"
#include "FindAudioComponent.h"
#include "BuildAudioComponentDescriptionForFilePlayer.h"
#include "BuildAudioComponentDescriptionForDefaultOutput.h"
#include "OpenAudioFile.h"
#include "AppState.h"
#include "GetAudioFileAudioStreamBasicDescription.h"
#include "GetNumberOfPackets.h"
#include "StopAudioOutputUnit.h"
#include "StopFilePlayerAudioUnit.h"
#include "CloseAudioFile.h"

void InstantiateFilePlayerAudioUnit(AudioUnit *filePlayerAudioUnit) {
  AudioComponentDescription filePlayerAudioComponentDescription = {0};
  
  BuildAudioComponentDescriptionForFilePlayer(&filePlayerAudioComponentDescription);
    
  AudioComponent filePlayerAudioComponent = FindAudioComponent(filePlayerAudioComponentDescription);
    
  CheckError(AudioComponentInstanceNew(filePlayerAudioComponent,
                                       filePlayerAudioUnit),
             "Instantiating the File Player Audio Unit");
  
  CheckError(AudioUnitInitialize(*filePlayerAudioUnit), "Initializing the filePlayerAudioUnit");
}

void CalculateNumberOfFramesToPlay(AppState *appState) {
  UInt64 packetsCount = 0;
  GetNumberOfPackets(appState->inputFile, "Getting the number of packets from the input file", &packetsCount);
  appState->numberOfInputFrames = packetsCount * appState->inputFormat.mFramesPerPacket;
}

void ScheduleTheWholeFileForPlayback(AudioUnit filePlayerAudioUnit, AppState *appState) {
  ScheduledAudioFileRegion rgn;
  
  memset(&(rgn.mTimeStamp), 0, sizeof(rgn.mTimeStamp));
  rgn.mTimeStamp.mFlags = kAudioTimeStampSampleTimeValid;
  rgn.mTimeStamp.mSampleTime = 0;
  
  rgn.mCompletionProc = NULL;
  rgn.mCompletionProcUserData = NULL;
  rgn.mAudioFile = appState->inputFile;
  rgn.mLoopCount = 0;
  rgn.mStartFrame = 0;
  rgn.mFramesToPlay = appState->numberOfInputFrames;
  
  CheckError(AudioUnitSetProperty(filePlayerAudioUnit,
                                  kAudioUnitProperty_ScheduledFileRegion,
                                  kAudioUnitScope_Global,
                                  0,
                                  &rgn,
                                  sizeof(rgn)),
             "Setting the audio unit property kAudioUnitProperty_ScheduledFileRegion");
}

void SetFilePlayerFileToPlay(AudioUnit filePlayerAudioUnit, AppState *appState) {
  CheckError(AudioUnitSetProperty(filePlayerAudioUnit,
                                  kAudioUnitProperty_ScheduledFileIDs,
                                  kAudioUnitScope_Global,
                                  0,
                                  &(appState->inputFile),
                                  sizeof(appState->inputFile)),
             "Audio Unit setting property value for kAudioUnitProperty_ScheduledFileIDs");
  
  CalculateNumberOfFramesToPlay(appState);
  
  ScheduleTheWholeFileForPlayback(filePlayerAudioUnit, appState);
}

void SetFilePlayerStartPlayingPoint(AudioUnit filePlayerAudioUnit) {
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
}

void CalculateFilePlayerOutputFrames(AudioUnit filePlayerAudioUnit, AppState *appState) {
  AudioStreamBasicDescription outputFormat = {0};
  UInt32 sizeOfOutputFormat = sizeof(AudioStreamBasicDescription);
  CheckError(AudioUnitGetProperty(filePlayerAudioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output,
                                  0,
                                  &outputFormat,
                                  &sizeOfOutputFormat),
             "Getting file player output format");
  appState->numberOfOutputFrames = appState->numberOfInputFrames * (outputFormat.mSampleRate / appState->inputFormat.mSampleRate);
}

AudioUnit SetUpFilePlayerAudioUnit(AppState *appState) {
  AudioUnit filePlayerAudioUnit;
  
  InstantiateFilePlayerAudioUnit(&filePlayerAudioUnit);
    
  SetFilePlayerFileToPlay(filePlayerAudioUnit, appState);
  
  SetFilePlayerStartPlayingPoint(filePlayerAudioUnit);
  
  CalculateFilePlayerOutputFrames(filePlayerAudioUnit, appState);
  
  return filePlayerAudioUnit;
}

OSStatus DefaultOutputAudioUnitRenderNotifyProc (void *inRefCon,
                                                 AudioUnitRenderActionFlags *ioActionFlags,
                                                 const AudioTimeStamp *inTimeStamp,
                                                 UInt32 inBusNumber,
                                                 UInt32 inNumberFrames,
                                                 AudioBufferList *ioData) {
  
  if (*ioActionFlags & kAudioUnitRenderAction_PostRender) {
    if (inTimeStamp->mFlags & kAudioTimeStampSampleTimeValid) {
      AppState *appState = (AppState *)inRefCon;
      if (appState->numberOfOutputFrames < inTimeStamp->mSampleTime) {
        pthread_mutex_lock(&(appState->mutex));
  
        appState->stopPlaying = true;
  
        pthread_cond_signal(&(appState->cond));
  
        pthread_mutex_unlock(&(appState->mutex));
      }
    }
  }
  
  return noErr;
}

AudioUnit SetUpDefaultOutputAudioUnit(AppState *appState) {
  AudioComponentDescription outputDefaultDescription = {0};
  
  BuildAudioComponentDescriptionForDefaultOutput(&outputDefaultDescription);
    
  AudioComponent defaultOutputAudioComponent = FindAudioComponent(outputDefaultDescription);
    
  AudioUnit defaultOutputAudioUnit;
    
  CheckError(AudioComponentInstanceNew(defaultOutputAudioComponent,
                                       &defaultOutputAudioUnit),
             "Instantiating the Default Output Audio Unit");
  
  CheckError(AudioUnitAddRenderNotify(defaultOutputAudioUnit,
                                      DefaultOutputAudioUnitRenderNotifyProc,
                                      appState),
             "Set notify proc for the default output audio unit render");

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

void WaitForPlaybackToFinish(AppState *appState) {
  pthread_mutex_lock(&(appState->mutex));
  while(!appState->stopPlaying) {
    pthread_cond_wait(&(appState->cond), &(appState->mutex));
  }
  pthread_mutex_unlock(&(appState->mutex));
}

void InitializeSynchronizationState(AppState *appState) {
  appState->stopPlaying = false;
  pthread_mutex_init(&(appState->mutex), NULL);
  pthread_cond_init(&(appState->cond), NULL);
}

void ReleaseResources(AppState *appState, AudioUnit filePlayerAudioUnit, AudioUnit defaultOutputAudioUnit) {
  StopAudioOutputUnit(defaultOutputAudioUnit);
  StopFilePlayerAudioUnit(filePlayerAudioUnit);
  CloseAudioFile(appState->inputFile);
}

int main(int argc, const char * argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "1st argument: You need to give the input file for playing back. You can use any Core Audio supported file such as .mp3, .aac, .m4a, .wav, .aif e.t.c.\n");
    return 1;
  }
  
  AppState appState = {0};
  
  OpenAudioFile(argv[1], &appState.inputFile);
  
  GetAudioFileAudioStreamBasicDescription(appState.inputFile, &appState.inputFormat);
    
  AudioUnit filePlayerAudioUnit = SetUpFilePlayerAudioUnit(&appState);

  AudioUnit defaultOutputAudioUnit = SetUpDefaultOutputAudioUnit(&appState);
  
  ConnectUnitsTogether(filePlayerAudioUnit, defaultOutputAudioUnit);
  
  InitializeSynchronizationState(&appState);
    
  StartDefaultOutputUnit(defaultOutputAudioUnit);
  
  WaitForPlaybackToFinish(&appState);
  
  ReleaseResources(&appState, defaultOutputAudioUnit, filePlayerAudioUnit);
  
  return 0;
}
