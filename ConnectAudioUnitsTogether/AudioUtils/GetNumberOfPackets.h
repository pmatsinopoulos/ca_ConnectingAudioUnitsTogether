//
//  GetNumberOfPackets.h
//  ConnectAudioUnitsTogether
//
//  Created by Panayotis Matsinopoulos on 30/7/21.
//

#ifndef GetNumberOfPackets_h
#define GetNumberOfPackets_h

#include <AudioToolbox/AudioToolbox.h>

void GetNumberOfPackets(AudioFileID iAudioFileID, const char *iMessage, UInt64 *oPacketCount);

#endif /* GetNumberOfPackets_h */
