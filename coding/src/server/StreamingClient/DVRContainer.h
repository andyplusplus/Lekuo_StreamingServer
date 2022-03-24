/*
#include "DVRContainer.h"
*/

#pragma once

#include "StdRsType.h"

class DVRContainer
{
public:
	DVRContainer(void);
	~DVRContainer(void);
	LK_Error Add_DVR(LPIDVR);
	LK_Error Del_DVR(LPIDVR);
	LK_Error Del_DVR(rsString);
private:
	DVR_map m_DVR_map;
};

