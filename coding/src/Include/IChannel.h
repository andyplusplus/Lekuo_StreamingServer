// IChannel.h: 
//
//////////////////////////////////////////////////////////////////////

#if !defined(LK_ICHANNEL_H_INCLUDED)
#define LK_ICHANNEL_H_INCLUDED

#include "LeKuo.h"
#include "IPlayer.h"
#include "IDataReceiver.h"

class IDVR;

typedef unsigned long MetadataLen;


class IChannel
{
public:

	virtual LK_Error Attach_DataReceiver(LPIDataReceiver inPlayer) = 0;
	virtual LPIDataReceiver Detach_DataReceiver() = 0;

	virtual LK_Error Play(const char* inPlayOption = 0) = 0;
	virtual BOOL IsPlaying() = 0;
	virtual LK_Error Stop() = 0;
	virtual LK_Error Destroy() = 0;

	virtual MetadataLen getMetadataLen() = 0;
	virtual MetadataLen getMetadata(unsigned char* outDataBuffer, MetadataLen inBufferLength) = 0;

	friend class IDVR;
	
protected:
	virtual ~IChannel() {};

};

typedef IChannel* LPIChannel;


#endif // !defined(LK_ICHANNEL_H_INCLUDED)

