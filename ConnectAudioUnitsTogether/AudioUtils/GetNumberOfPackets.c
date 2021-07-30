//
//  GetNumberOfPackets.c
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#include <AudioToolbox/AudioToolbox.h>
#include "GetNumberOfPackets.h"
#include "CheckError.h"

void GetNumberOfPackets(AudioFileID iAudioFileID, const char *iMessage, UInt64 *oPacketCount) {
  UInt32 isWriteable = 0;
  UInt32 propertyValueSize = sizeof(UInt32);
  CheckError(AudioFileGetPropertyInfo(iAudioFileID,
                                      kAudioFilePropertyAudioDataPacketCount,
                                      &propertyValueSize,
                                      &isWriteable),
             iMessage);
  
  CheckError(AudioFileGetProperty(iAudioFileID,
                                  kAudioFilePropertyAudioDataPacketCount,
                                  &propertyValueSize,
                                  oPacketCount),
             "Getting the total number of packets");
}
