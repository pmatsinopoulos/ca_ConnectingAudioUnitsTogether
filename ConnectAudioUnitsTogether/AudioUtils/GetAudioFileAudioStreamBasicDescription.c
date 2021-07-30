//
//  GetAudioFileAudioStreamBasicDescription.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include "GetAudioFileAudioStreamBasicDescription.h"
#include "CheckError.h"

void GetAudioFileAudioStreamBasicDescription(AudioFileID iAudioFileHandler,
                                             AudioStreamBasicDescription *oAudioFormat) {
  UInt32 propertyValueSize = {0};
  UInt32 isWritable = {0};
  CheckError(AudioFileGetPropertyInfo(iAudioFileHandler,
                                      kAudioFilePropertyDataFormat,
                                      &propertyValueSize,
                                      &isWritable),
             "Reading the size of the property kAudioFilePropertyDataFormat");
  
  CheckError(AudioFileGetProperty(iAudioFileHandler,
                                  kAudioFilePropertyDataFormat,
                                  &propertyValueSize,
                                  oAudioFormat),
             "Reading the value of the property kAudioFilePropertyDataFormat");
}
