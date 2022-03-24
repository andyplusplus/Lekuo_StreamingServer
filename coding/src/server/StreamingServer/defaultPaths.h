/*
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright c 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 the 'License'. You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */

/*
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 */

/*
 * defaultPaths.h - define the default paths to hardcode into the executables
 * 
 * IMPORTANT NOTE : The trailering directory separators are required on all 
 *                  DEFAULTPATHS_*_DIR* defines
 *
 * Contributed by: Peter Bray
 */

#ifdef __Win32__
#include "StrPtrLen.h"
# define DEFAULTPATHS_DIRECTORY_SEPARATOR	"\\"

//c:\Program Files\\lekuo\\StreamingServer\\Modules
	  
extern char* DEFAULTPATHS_ETC_DIR			 ;//"c:\Program Files\\lekuo\\StreamingServer\\"
//extern char* DEFAULTPATHS_ETC_DIR_OLD		 ;//"c:\Program Files\\lekuo\\StreamingServer\\"
extern char* DEFAULTPATHS_SSM_DIR			 ;//"c:\Program Files\\lekuo\\StreamingServer\\Modules\\"
extern char* DEFAULTPATHS_LOG_DIR			 ;//"c:\Program Files\\lekuo\\StreamingServer\\Logs\\"
extern char* DEFAULTPATHS_MOVIES_DIR         ;//"c:\Program Files\\lekuo\\StreamingServer\\Movies\\"
extern char* DEFAULTPATHS_PID_FILE           ;//""
extern char* DEFAULTPATHS_PID_DIR			 ;//""

extern StrPtrLenDel   DEFAULTPATHS_ETC_DIR_Del		;
//extern StrPtrLenDel   DEFAULTPATHS_ETC_DIR_OLD_Del	;
extern StrPtrLenDel   DEFAULTPATHS_SSM_DIR_Del		;
extern StrPtrLenDel   DEFAULTPATHS_LOG_DIR_Del		;
extern StrPtrLenDel   DEFAULTPATHS_MOVIES_DIR_Del	;
extern StrPtrLenDel   DEFAULTPATHS_PID_FILE_Del     ;
extern StrPtrLenDel   DEFAULTPATHS_PID_DIR_Del      ;


//
extern char* DEFAULTPATHS_XMLFilePathName					;//c:\\Program Files\\lekuo\\StreamingServer\\streamingserver.xml
extern char* DEFAULTPATHS_ConfigFileName					;//c:\\Program Files\\lekuo\\StreamingServer\\streamingserver.cfg
extern StrPtrLenDel   DEFAULTPATHS_XMLFilePathName_Del		;
extern StrPtrLenDel   DEFAULTPATHS_ConfigFileName_Del		;
//
	#ifdef __cplusplus
	extern "C" {
	#endif

		//Read Path Related Initialize from DVR.ini File
		void DEFAULTPATHS_Initialize();

		//按psItemName的不同，改变参数psItemValue为绝对路径
		int DEFAULTPATHS_GetAbsoluteDir(const char* psPathName, char** psPath);			

		//Set the psPathDest according to psPathSrc
		int DEFAULTPATHS_GetRelativeDir(char* psPathName, char* psPathSrc, char* psPathDest);


	#ifdef __cplusplus
	}
	#endif


#elif __MacOSX__

# define DEFAULTPATHS_DIRECTORY_SEPARATOR	"/"

# define DEFAULTPATHS_ETC_DIR			"/Library/QuickTimeStreaming/Config/"
# define DEFAULTPATHS_ETC_DIR_OLD		"/etc/"
# define DEFAULTPATHS_SSM_DIR			"/Library/QuickTimeStreaming/Modules/"
# define DEFAULTPATHS_LOG_DIR			"/Library/QuickTimeStreaming/Logs/"
# define DEFAULTPATHS_MOVIES_DIR        "/Library/QuickTimeStreaming/Movies/"
# define DEFAULTPATHS_PID_DIR           "/var/run/"

#else

# define DEFAULTPATHS_DIRECTORY_SEPARATOR	"/"

# define DEFAULTPATHS_ETC_DIR			"/etc/streaming/"
# define DEFAULTPATHS_ETC_DIR_OLD		"/etc/"
# define DEFAULTPATHS_SSM_DIR			"/usr/local/sbin/StreamingServerModules/"
# define DEFAULTPATHS_LOG_DIR			"/var/streaming/logs/"
# define DEFAULTPATHS_MOVIES_DIR        "/usr/local/movies/"
# define DEFAULTPATHS_PID_DIR           "/var/run/"

#endif

