/*
#include "StdRsType.h"
*/

#ifndef _STDRSTYPE_INCLUDED__
#define _STDRSTYPE_INCLUDED__

#include "IChannel.h"
#include "IDVR.h"
#include "IPlayer.h"

#pragma warning (push, 3)		//Disable STL Warning

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <list>

#include "ILkTokenParser.h"


#pragma region STL types

	typedef		std::string							rsString;



	typedef		std::set<LPIDVR>					DVR_set;
	typedef		std::set<LPIDVR>::iterator 			DVR_set_iter;

	typedef		std::vector<LPIDVR>					DVR_vector;
	typedef		std::vector<LPIDVR>::iterator 		DVR_vector_iter;


	typedef		std::map<rsString, LPIDVR>			DVR_map;
	typedef		DVR_map::iterator					DVR_map_iter;

	typedef		std::map<rsString, LPIChannel>		Channel_map;
	typedef		Channel_map::iterator				Channel_map_iter;



	typedef		std::vector<LPIChannel>				Channel_vec;
	typedef		Channel_vec::iterator				Channel_vec_iter;

	typedef		std::vector<HWND>					HWND_vec;
	typedef		HWND_vec::iterator					HWND_vec_iter;

	typedef		std::set<LPIPlayer>					Player_set;
	typedef		Player_set::iterator				Player_set_iter;

	typedef		std::vector<LPIPlayer>				Player_vector;
	typedef		Player_vector::iterator				Player_vector_iter;

	typedef		std::map<rsString, rsString>		String_map;
	typedef		String_map::iterator				String_map_iter;

#pragma endregion STL types

#pragma warning (pop)

#endif //_STDRSTYPE_INCLUDED__