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
 ��ȫ���ֵ��ṩ�˴���Ϣ����-->ʵ���ı���Ϣ��ӳ��, ��Щ��Ϣ������Ԥ����Ľṹ,  �Ӷ�ʹ�÷����������ױ��ػ�
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
		// �ú�������dictionary map, �����ڳ�ʼ����һ��RTSPMessages����ǰ����
        static void Initialize();
		//�ù��캯�������Ĺ�����Ҫ�ǱȽ�message.txt�еĲ����Ƿ���Ԥ����Ĳ�����ͬ, �ĵ�message.txt�и���Ϣ, ��ʹ��message.txt����Ϣ
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
