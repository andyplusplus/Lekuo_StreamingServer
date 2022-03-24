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
		"ConfParser.cpp"
*/


#include "ConfParser.h"
#include "OSMemory.h"

#include "MyAssert.h"


#include <stdlib.h> 

#include "GetWord.h"
#include "Trim.h"


#include <string.h> 
#include <stdio.h>  


static Bool16 SampleConfigSetter( const char* paramName, const char* paramValue[], void* userData );
static void DisplayConfigErr( const char*fname, int lineCount, const char*lineBuff, const char *errMessage );



void TestParseConfigFile()
{
     ParseConfigFile( false, "qtss.conf", SampleConfigSetter, NULL );

}

static Bool16 SampleConfigSetter( const char* paramName, const char* paramValue[], void* /*userData*/ )
{
    qtss_printf( "param: %s", paramName );
    
    int x = 0;
    
    while ( paramValue[x] )
    {
        qtss_printf( " value(%li): %s ", (long)x, paramValue[x] );
        x++;
    }
    
    qtss_printf( "\n" );
    
    return false;
}


static void DisplayConfigErr( const char*fname, int lineCount, const char*lineBuff, const char *errMessage )
{
    
    qtss_printf( "- Configuration file error:\n" );
    
    
    if ( lineCount )
        qtss_printf( "  file: %s, line# %i\n", fname, lineCount );
    else
        qtss_printf( "  file: %s\n", fname );
    
    if ( lineBuff )
        qtss_printf( "  text: %s", lineBuff ); // lineBuff already includes a \n
    
    if ( errMessage )
        qtss_printf( "  reason: %s\n", errMessage ); // lineBuff already includes a \n
}

//对配置文件进行解析
int ParseConfigFile( 
    Bool16  allowNullValues	//是否允许空值
    , const char* fname		//文件名
    , Bool16 (*ConfigSetter)( const char* paramName, const char* paramValue[], void* userData )
    , void* userData )
{
    int     error = -1;
    FILE    *configFile;
    int     lineCount = 0;

    Assert( fname );
    Assert( ConfigSetter );
    
    
    if (!fname) return error;
    if (!ConfigSetter) return error;
    
    //1. 只读方式打开文件
    configFile = fopen( fname, "r" );
    
	//  Assert( configFile );
    
    if ( configFile )
    {
        long    lineBuffSize = kConfParserMaxLineSize;
        long    wordBuffSize = kConfParserMaxParamSize;
        
        
        char    lineBuff[kConfParserMaxLineSize];
        char    wordBuff[kConfParserMaxParamSize];
        
        char    *next;
        
        // debug assistance -- CW debugger won't display large char arrays as strings
        //char* l = lineBuff;
        //char* w = wordBuff;
        
     //2. 读取每一行, 并进行分析
        do 
        {   
            next = lineBuff;
            
            // get a line ( fgets adds \n+ 0x00 )
         //2.1 读取一行   
            if ( fgets( lineBuff, lineBuffSize, configFile ) == NULL )
                break;
            
            lineCount++;
            error = 0; // allow empty lines at beginning of file.

            // trim the leading whitespace
            next = TrimLeft( lineBuff );
                
            if (*next)
            {   
                                
                if ( *next == '#' )
                {
                    // it's a comment
                    // prabably do nothing in release version?

                    //  qtss_printf( "comment: %s" , &lineBuff[1] );
                    
                    error = 0;
                    
                }
                else
                {   char* param;
                
                    // grab the param name, quoted or not
		 //2.2. 提取出一行的关键字, 并赋值给param
                    if ( *next == '"' )
                        next = GetQuotedWord( wordBuff, next, wordBuffSize );
                    else
                        next = GetWord( wordBuff, next, wordBuffSize );
                        
                    Assert( *wordBuff );
                    
                    param = NEW char[strlen( wordBuff ) + 1 ];
                    
                    Assert( param );
                    
                    if ( param )
                    {
         //2.3. 解析出一行中的所有值, 并把这些值存入values
                        const char* values[kConfParserMaxParamValues+1];	//字符串数据
                        int         maxValues = 0;
                        
                        strcpy( param, wordBuff );
                                            

                        values[maxValues] = NULL;
						//2.3.1. 以下这段循环用于解析出所有的值      
                        while ( maxValues < kConfParserMaxParamValues && *next )
                        {
                            // ace
                            next = TrimLeft( next );
                            
                            if (*next)
                            {
                                if ( *next == '"' )
                                    next = GetQuotedWord( wordBuff, next, wordBuffSize );
                                else
                                    next = GetWord( wordBuff, next, wordBuffSize );
                            
                                    char* value = NEW char[strlen( wordBuff ) + 1 ];
                                    
                                    Assert( value );
                                    
                                    if ( value )
                                    {
                                        strcpy( value, wordBuff );
                                        
                                        values[maxValues++] = value;
                                        values[maxValues] = 0;
                                    }
                                            
                            }
                        
                        }//while ( maxValues < kConfParserMaxParamValues && *next )
                    
                        if ( (maxValues > 0 || allowNullValues) && !(*ConfigSetter)( param,  values, userData ) )  //param: 指参数名, values: 参数数组, userData:??
                            error = 0;
						    // 这里执行完后,该程序结束
                        else
                        {   error = -1;
                            if ( maxValues > 0 )
                                DisplayConfigErr( fname, lineCount, lineBuff, "Parameter could not be set." );
                            else
                                DisplayConfigErr( fname, lineCount, lineBuff, "No value to set." );
                        }
                        
                        delete [] param;
                        
                        maxValues = 0;
                        
                        while ( values[maxValues] )
                        {   char** tempValues = (char**)values; // Need to do this to delete a const
                            delete [] tempValues[maxValues];
                            maxValues++;
                        }
                        
                    
                    }  //if ( param ), 如果这一行是参数名
                    
                } //if ( *next == '#' )的Else部分, 即该行不是注释行
                    
            
            }// if (*next), 如果读出的一行非空
        
        
        
        } while ( feof( configFile ) == 0 && error == 0 );	//起点在110行以前
    
        (void)fclose(  configFile  );
    }
//  else
//  {
//      qtss_printf("Couldn't open config file at: %s\n", fname);
//  }
    
    return error;

}
