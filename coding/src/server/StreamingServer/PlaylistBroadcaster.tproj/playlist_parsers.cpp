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
       playlist_parsers.cpp
*/


#include "playlist_parsers.h"

char* SDPFileParser::sMediaTag = "m";
char* SDPFileParser::sAttributeTag  = "a";
char* SDPFileParser::sConnectionTag = "c";

SDPFileParser::~SDPFileParser()
{
    if (fSDPBuff) 
    {   delete[] fSDPBuff;
        fSDPBuff = NULL;
    }
}

//是否注释
bool SDPFileParser::IsCommented(SimpleString *theString)
{
    if ( NULL == theString) return false;
    if ( theString->fLen == 0) return false;  
    if ( theString->fTheString[0] == '#' ) return true; // It's commented if the first non-white char is #

    return false;
}


//把textStrPtr解析到fWords数组中
int TextLine::Parse (SimpleString *textStrPtr)
{
    short count = 0;
    
    do
    {
        if (!textStrPtr) break;

        count = CountDelimeters(textStrPtr,sWordDelimeters);
        if (count < 1) break;
        
        fWords.SetSize(count);      
        fSource = *textStrPtr;      
        SimpleString    *listStringPtr = fWords.Begin();
        
        SimpleString    currentString;
        currentString.SetString(textStrPtr->fTheString, 0);
        
        for ( short i = 0; i < count; i ++)
        {   GetNextThing(textStrPtr,&currentString, sWordDelimeters, &currentString);
            *listStringPtr = currentString;
            listStringPtr++;
        } 

    } while (false);
        
    
    return count;
}

//解析多行
int LineAndWordsParser::Parse (SimpleString *textStrPtr)
{
    short           count = 0;  
    
    do
    {
        if (!textStrPtr) break;
                    
        count = CountDelimeters(textStrPtr,sLineDelimeters);
        if (count < 1) break;

        fLines.SetSize(count);      
        fSource = *textStrPtr;
        TextLine        *listStringPtr = fLines.Begin();
            
        SimpleString    currentString;
        currentString.SetString(textStrPtr->fTheString, 0);
        
        for ( short i = 0; i < count; i ++)
        {   GetNextThing(textStrPtr,&currentString, sLineDelimeters, &currentString);
            listStringPtr->Parse(&currentString);
            listStringPtr++;
        } 
    } while (false);
    
    return count;
}

//统计行数
short SDPFileParser::CountQTTextLines() 
{
    short numlines = 0; 
    TextLine        *theLinePtr = fParser.fLines.Begin();

    while (theLinePtr)
    {   if (GetQTTextFromLine(theLinePtr))
            numlines ++;
        
        theLinePtr = fParser.fLines.Next();
    };
    
    return numlines;
}

//统计媒体项数
short SDPFileParser::CountMediaEntries() 
{
    bool commented = false; 
    bool isEqual = false;
    short numTracks = 0;
    
    TextLine        *theLinePtr = fParser.fLines.Begin();
    SimpleString    *firstWordPtr;

    while (theLinePtr)
    {   
        do 
        {   firstWordPtr = theLinePtr->fWords.Begin();
            if (!firstWordPtr) break;
                        
            commented = IsCommented(firstWordPtr);
            if (commented) break;
    
            isEqual = Compare(firstWordPtr, SDPFileParser::sMediaTag, true);
            if (!isEqual) break;
             
            numTracks ++;

        } while (false);
        
        theLinePtr = fParser.fLines.Next();
    };
    
    return numTracks;
}

//计算rtpmap数
short SDPFileParser::CountRTPMapEntries() 
{
    short startPos = fParser.fLines.GetPos();
    short result = 0;
    TextLine *theLinePtr = fParser.fLines.Get();
    SimpleString mapString("rtpmap"); 
    SimpleString *aWordPtr;
    bool isEqual;
    
    while (theLinePtr)
    {   
        aWordPtr = theLinePtr->fWords.Begin();
        if (aWordPtr)           
        {
            isEqual = Compare(aWordPtr, SDPFileParser::sAttributeTag, true);
            if (isEqual)  // see if this attribute is a rtpmap line
            {   
                aWordPtr = theLinePtr->fWords.SetPos(1);            
                isEqual = Compare(aWordPtr, &mapString, false);
                if (isEqual) result ++;
            }
            else // could be a comment or some other attribute
            {   isEqual = Compare(aWordPtr, SDPFileParser::sMediaTag, true);
                if (isEqual) break; // its another media line so stop
            }
        }
        theLinePtr = fParser.fLines.Next();
    };
    
    fParser.fLines.SetPos(startPos);
    
    return result;
}

//从m=video 5004 RTP/AVP 96获得协议号
void SDPFileParser::GetPayLoadsFromLine(TextLine *theLinePtr, TypeMap *theTypeMapPtr)
{
    short count = 0;
    if (theLinePtr == NULL || theTypeMapPtr == NULL)
        return;

    SimpleString *aWordPtr = theLinePtr->fWords.SetPos(5);// get protocol ID str
    //计算协议数
	while (aWordPtr)
    {   count ++;
        aWordPtr = theLinePtr->fWords.Next();// get next protocol ID str
    }   
	//设置协议数组长度
    theTypeMapPtr->fPayLoads.SetSize(count);    
    short* idPtr = theTypeMapPtr->fPayLoads.Begin();// get protocol ID ref
    aWordPtr = theLinePtr->fWords.SetPos(5);// get protocol ID str
	//把相关值读入协议数组
    while (aWordPtr && idPtr)
	{
     	*idPtr = (short) aWordPtr->GetInt();
        aWordPtr = theLinePtr->fWords.Next();// get next protocol ID str
        idPtr = theTypeMapPtr->fPayLoads.Next();// get next protocol ID ref
    }
}

//获得QuickTimeText
bool SDPFileParser::GetQTTextFromLine(TextLine *theLinePtr)
{
//a=x-qt-text-cpy:xxxxx
//a=x-qt-text-nam:xxxxxx
//a=x-qt-text-inf:xxxxxxx

    bool result = false;
    SimpleString *aWordPtr; 
    char *xString ="a=x-qt-text";
    do 
    {
        aWordPtr = theLinePtr->fWords.Begin();
        if (!aWordPtr) break;
        
        bool isEqual = (0 == strncmp(aWordPtr->fTheString, xString,strlen(xString) ) ) ? true: false;
        if (!isEqual) break;
        
        result = true;
    
    } while (false);
    
    return result;
}

//解析m=video 5004 RTP/AVP 96, 并把相关值读入TypeMap数据结构
bool SDPFileParser::GetMediaFromLine(TextLine *theLinePtr, TypeMap *theTypeMapPtr)
{
    bool result = false;
    SimpleString *aWordPtr; 

    do 
    {
        aWordPtr = theLinePtr->fWords.Begin();
        if (!aWordPtr) break;
            
        bool isEqual = Compare(aWordPtr, SDPFileParser::sMediaTag, true);
        if (!isEqual) break;

        aWordPtr = theLinePtr->fWords.SetPos(1);// get type 
        if (!aWordPtr) break;
        
        theTypeMapPtr->fTheTypeStr = *aWordPtr;
                
        aWordPtr = theLinePtr->fWords.SetPos(2);// get movie port 
        if (!aWordPtr) break;
        
		theTypeMapPtr->fPort = aWordPtr->GetInt();
        
        aWordPtr = theLinePtr->fWords.SetPos(3);// get protocol 
        if (!aWordPtr) break;
        
        theTypeMapPtr->fProtocolStr = *aWordPtr;

        GetPayLoadsFromLine(theLinePtr, theTypeMapPtr);
                
        result = true;
    } while (false);
    
    return result;
    
}

//由SDP的SDP的a=rtpmap:97 X-QDM/22050/2 解析得到PayLoad
bool SDPFileParser::GetRTPMap(TextLine *theLinePtr,PayLoad *payloadPtr)
{
    bool lineOK = false;
    SimpleString *aWordPtr;
    
    do
    {       
        if (!theLinePtr || !payloadPtr) break;
        
        aWordPtr = theLinePtr->fWords.SetPos(2); // the Payload ID
        if (!aWordPtr) break;   
		payloadPtr->payloadID = aWordPtr->GetInt();
    
        aWordPtr = theLinePtr->fWords.Next(); // the Payload type string
        if (!aWordPtr) break;
        payloadPtr->payLoadString = *aWordPtr;
        
        payloadPtr->timeScale = 0;
		aWordPtr = theLinePtr->fWords.Next(); // the Payload timeScale
		if (aWordPtr)
            payloadPtr->timeScale = aWordPtr->GetInt();
        
        lineOK = true;
        
    } while (false);
    
    return lineOK;

}

//获得SDP的RTPMap行, 并解析这些行
TextLine *SDPFileParser::GetRTPMapLines(TextLine *theLinePtr,TypeMap *theTypeMapPtr)
{ 
    
    do
    {       
        if (!theLinePtr || !theTypeMapPtr) break;
            
        short numAttributes = CountRTPMapEntries();
        theTypeMapPtr->fPayLoadTypes.SetSize(numAttributes);
        PayLoad *payloadPtr = theTypeMapPtr->fPayLoadTypes.Begin(); 
        
		while( theLinePtr && payloadPtr && (numAttributes > 0) ) 
        {
            bool haveMAP = GetRTPMap(theLinePtr,payloadPtr); 
            if (haveMAP)
            {   numAttributes --;
                payloadPtr = theTypeMapPtr->fPayLoadTypes.Next(); //skip to next payload entry
            }

            theLinePtr = fParser.fLines.Next(); // skip to next line
	        //stop checking if this is a new media line
			if(theLinePtr == NULL || Compare(theLinePtr->fWords.Begin(), SDPFileParser::sMediaTag, true)) 
                break;
            
        }
    } while (false);
    
    return theLinePtr;
}

//由SDP的a=control:trackID=1统计得到trackID
TextLine * SDPFileParser::GetTrackID(TextLine *theLinePtr,TypeMap *theTypeMapPtr)
{
    SimpleString *aFieldPtr;
	SimpleString *aWordPtr;
    Bool16 foundID = false;

	while(theLinePtr && !foundID)
	{
        if(Compare(theLinePtr->fWords.Begin(), SDPFileParser::sMediaTag, true)) //stop checking if this is a new media line
           break;

        do 
        {
            SimpleString controlString("control"); 
            
            aFieldPtr = theLinePtr->fWords.Begin();
            if (!aFieldPtr) break;
            
            bool isEqual = Compare(aFieldPtr, SDPFileParser::sAttributeTag, true);
            if (!isEqual) break;
            
            aWordPtr = theLinePtr->fWords.SetPos(1);			
            if (!aWordPtr) break;
            
            isEqual = Compare(aWordPtr, &controlString, false);
            if (!isEqual) break;
            
            aWordPtr = theLinePtr->fWords.SetPos(3);			
            if (!aWordPtr) break;
            
            theTypeMapPtr->fTrackID = aWordPtr->GetInt();
            foundID = true;

		} while (false);
	

		theLinePtr = fParser.fLines.Next();

	}
	
	return theLinePtr;

}

//解析IP地址
bool SDPFileParser::ParseIPString(TextLine *theLinePtr)
{
    bool    result = false;
    SimpleString *aWordPtr;
    do 
    {        
        SimpleString ipIDString("IP4"); 
        
        aWordPtr = theLinePtr->fWords.Begin();
        if (!aWordPtr) break;
            
        bool isEqual = Compare(aWordPtr,SDPFileParser::sConnectionTag, true);
        if (!isEqual) break;

        aWordPtr = theLinePtr->fWords.SetPos(2);            
        if (!aWordPtr) break;
        
        isEqual = Compare(aWordPtr, &ipIDString, false);
        if (!isEqual) break;

        aWordPtr = theLinePtr->fWords.SetPos(3);            
        if (!aWordPtr) break;
        
        fIPAddressString.SetString(aWordPtr->fTheString, aWordPtr->fLen);
        result = true;
        
    } while (false);
        
    return result;

}

//解析SDP文件, 该函数会调用所有的其它函数
SInt32 SDPFileParser::ParseSDP(char *theBuff) 
{
    SInt32 result = 0;  
    bool found = false;
    
    SimpleString source(theBuff);
    fSource.SetString( theBuff, strlen(theBuff) );
    fParser.Parse(&source);
    

//  Test parse
#if 0
    qtss_printf("-----------------------------------------------------\n");
    char tempString[256];
    TextLine *theLine = fParser.fLines.Begin();
    while (theLine)
    {   SimpleString *theWord = theLine->fWords.Begin();
        while (theWord)
        {   theWord->GetString(tempString,256);
            qtss_printf(tempString);
            theWord = theLine->fWords.Next();
            if (theWord) qtss_printf(" _ ");
        }
        theLine = fParser.fLines.Next();
        qtss_printf("\n");
    }
    // exit (0);
#endif

    fNumQTTextLines = CountQTTextLines();
    fQTTextLines.SetSize( (SInt16) fNumQTTextLines);
    SimpleString *theQTTextPtr = fQTTextLines.Begin();
    
    fNumTracks = CountMediaEntries();   
    fSDPMediaList.SetSize((SInt16) fNumTracks);
    
    TextLine *theLinePtr = fParser.fLines.Begin();
    TypeMap *theTypeMapPtr = fSDPMediaList.Begin();
    
    bool foundIP = false;
    while (theLinePtr && theTypeMapPtr)
    {   
        if (foundIP == false)
        {   foundIP = ParseIPString(theLinePtr);
        }

        if (theQTTextPtr && GetQTTextFromLine(theLinePtr))
        {   SimpleString *srcLinePtr = theLinePtr->fWords.Begin();          
            theQTTextPtr->SetString(srcLinePtr->fTheString, strcspn(srcLinePtr->fTheString, "\r\n") );
            theQTTextPtr = fQTTextLines.Next();
        }
        
        found = GetMediaFromLine(theLinePtr, theTypeMapPtr);
		if (found)
		{
			theLinePtr = fParser.fLines.Next();
			if (!theLinePtr) break;	// no more lines to process
		
            int startLine = fParser.fLines.GetPos();

            theLinePtr = fParser.fLines.SetPos(startLine);
			(void) GetRTPMapLines(theLinePtr,theTypeMapPtr);			
 
            theLinePtr = fParser.fLines.SetPos(startLine);
		    (void) GetTrackID(theLinePtr,theTypeMapPtr);

            theLinePtr = fParser.fLines.SetPos(startLine);
			theTypeMapPtr = fSDPMediaList.Next();	
            continue;
		}
        
        theLinePtr = fParser.fLines.Next();
    }
    
    return result;
}


//由SDP文件名, 读入SDP参数
SInt32 SDPFileParser::ReadSDP(char *filename)
{
    int result = -1;
    long int bytes= 0;
    
    FILE *f = NULL;
    
    if (fSDPBuff != NULL) 
    {   delete[] fSDPBuff;
        fSDPBuff = NULL;
    }
    
    do 
    {
        f = ::fopen(filename, "r");
        if (f == NULL) break;
                
        result = 1;
        result = ::fseek(f, 0, SEEK_SET);
        if (result != 0) break;
        
        fSDPBuff = new char[cMaxBytes + 1];
        if (NULL == fSDPBuff) break;
        fSDPBuff[cMaxBytes] = 0;
        
        bytes = ::fread(fSDPBuff, sizeof(char), cMaxBytes, f);
        if (bytes < 1) break;
        fSDPBuff[bytes] = 0;
        
        result = ParseSDP(fSDPBuff);
        if (result != 0) break;
        
        result = 0;
        
    } while (false);
        
    if (f != NULL) 
    {   ::fclose (f);
        f = NULL;
    }
    
    return result;
}


