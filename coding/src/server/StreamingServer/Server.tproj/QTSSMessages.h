/*
 *
 * APPLE_LICENSE_HEADER_START@
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
    File:       QTSSMessages.h

    Contains:   This global dictionary provides a central mapping from message
                names to actual text messages, stored in the provided prefs source.
                
                This allows the whole server to be easily localizeable.
 该全局字典提供了从消息名字-->实际文本消息的映射, 这些消息存贮在预定义的结构,  从而使得服务器很容易本地化
*/

#ifndef __QTSSMESSAGES_H__
#define __QTSSMESSAGES_H__

#include "QTSS.h"
#include "QTSSDictionary.h"
#include "PrefsSource.h"

class QTSSMessages : public QTSSDictionary
{
    public:
    
        // INITIALIZE
        //
        // This function sets up the dictionary map. Must be called before instantiating
        // the first RTSPMessages object.
		// 该函数设置dictionary map, 必须在初始化第一个RTSPMessages对象前调用
        static void Initialize();
		//该构造函数所做的工作主要是比较message.txt中的参数是否与预编译的参数相同, 哪但message.txt有该消息, 则使用message.txt的消息
        QTSSMessages(PrefsSource* inMessages);
        virtual ~QTSSMessages() {}
        

        //Use the standard GetAttribute method in QTSSDictionary to retrieve messages
        
    private:
    
        enum
        {
            kNumMessages = 74 // 0 based count so it is one more than last message index number
        };
    
        static char*        sMessagesKeyStrings[];
        static char*        sMessages[];
};


#endif // __QTSSMESSAGES_H__
