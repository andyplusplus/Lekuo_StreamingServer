// IPlayer.h: interface for the IPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LK_IPLAYER_H_INCLUDED)
#define LK_IPLAYER_H_INCLUDED

#include "LeKuo.h"
#include "IDataReceiver.h"


typedef	HWND PlayControlType;
const   HWND PlayControlType_NULL = NULL;	//the null value of PlayControlType


class IPlayer : public IDataReceiver
{
public:

	virtual ~IPlayer() {};

	virtual LK_Error Attach_PlayControl(PlayControlType inWindow) = 0;
	virtual PlayControlType Detach_PlayControl() = 0;
};

typedef IPlayer*  LPIPlayer;


#endif // !defined(LK_IPLAYER_H_INCLUDED)
