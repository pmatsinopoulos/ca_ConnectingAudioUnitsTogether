//
//  OpenAudioFile.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "CheckError.h"
#include "OpenAudioFile.h"

void OpenAudioFile(const char *iFileName, AudioFileID *oAudioFileID) {
  CFStringRef strRef = CFStringCreateWithCString(kCFAllocatorDefault, iFileName, CFStringGetSystemEncoding());
  CFURLRef urlRef = CFURLCreateWithString(kCFAllocatorDefault, strRef, NULL);
  
  CheckError(AudioFileOpenURL(urlRef,
                              kAudioFileReadPermission,
                              0,
                              oAudioFileID),
             "Opening the audio file");
  
  CFRelease(strRef);
  CFRelease(urlRef);
}
