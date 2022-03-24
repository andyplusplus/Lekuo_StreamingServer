//  
//   LogPriorityLevel.h
//

#ifndef LogPriorityLevel_H
#define LogPriorityLevel_H


typedef enum {
	PL_EMERG = 0, 
		PL_FATAL = 0,
		PL_ALERT = 100,
		PL_CRITC = 200,
		PL_ERROR = 300,
		PL_WARNG = 400,
		PL_NOTIC = 500,
		PL_INFOO = 600,
		PL_DEBUG = 700,
		PL_NOTST = 800
} PriorityLevel;

#endif  //LogPriorityLevel_H