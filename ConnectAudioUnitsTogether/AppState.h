//
//  AppState.h
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#ifndef PlayerData_h
#define PlayerData_h

#include <CoreAudio/CoreAudio.h>
#include <pthread/pthread.h>

typedef struct AppState {
  // synchronization related:
  Boolean stopPlaying;
  pthread_cond_t cond;
  pthread_mutex_t mutex;

  AudioStreamBasicDescription inputFormat;
  AudioFileID                 inputFile;
  Float64                     numberOfInputFrames;
  Float64                     numberOfOutputFrames;
} AppState;


#endif /* AppState_h */
