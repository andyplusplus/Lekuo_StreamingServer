//"playerAdaptor.h"

#if !defined(PLAYER_ADAPTOR_H_INCLUDED)
#define      PLAYER_RSADAPTOR_H_INCLUDED

#ifdef LEKUO_PLAYERADAPTOR_EXPORT
#	define PLAYEADAPTOR_API   extern "C"	__declspec(dllexport) 
#else
#	define PLAYEADAPTOR_API	extern "C"	__declspec(dllimport)
#endif

#include "IPlayer.h"


PLAYEADAPTOR_API LPIPlayer	CreatePlayer(const char* inStr, Tag inTag);

typedef LPIPlayer	(*CreatePlayer_Ptr)(const char* inStr, Tag inTag);



#endif	//PLAYER_ADAPTOR_H_INCLUDED
