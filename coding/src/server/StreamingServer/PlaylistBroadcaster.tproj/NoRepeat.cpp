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
   NoRepeat.cpp
 */


#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>


#include "NoRepeat.h"


//构造函数, 设置最大的允许结点数
NoRepeat::NoRepeat ( UInt32 numNoRepeats )
    : PLDoubleLinkedList<SimplePlayListElement>()
{
    mMaxElements = numNoRepeats;
}

NoRepeat::~NoRepeat()
{
    // we have nothing to do, just let the PLDoubleLinkedList clear itself
}

//按名字比较
bool NoRepeat::CompareNameToElement( PLDoubleLinkedListNode<SimplePlayListElement>* node, void* name )
{
    if ( !::strcmp( node->fElement->mElementName, (const char*)name ) )
        return true;
    
    return false;
}

//该名字的结点是否在列表中
bool NoRepeat::IsInList( char* name )
{
    PLDoubleLinkedListNode<SimplePlayListElement>* whichElement;
    
    whichElement = ForEachUntil( CompareNameToElement, (void*)name );  //CompareNameToElement为比较的函数指针
    
    if ( whichElement )
        return true;
    
    return false;
    
}

//把结点加入列表
PLDoubleLinkedListNode<SimplePlayListElement>* NoRepeat::AddToList( PLDoubleLinkedListNode<SimplePlayListElement>* node )
{
    AddNode(node);
    PLDoubleLinkedListNode<SimplePlayListElement>* oldTail = NULL;
    
    if ( fNumNodes > mMaxElements )
    {   oldTail = fTail;
        this->RemoveNode( fTail );
    }
    
    return oldTail;
}

//把特定名字的结点入到列表中,包括两个步骤:
//   1. 为新结点申请空间
//   2. 把该结点加入到列表
bool NoRepeat::AddToList( char* name )
{
    Assert( false );
    
    bool addedSuccesfully = false;

    if ( !this->IsInList( name ) )
    {
        SimplePlayListElement*                          element;
        PLDoubleLinkedListNode<SimplePlayListElement>*  node = NULL;

        element = new SimplePlayListElement(name);
        
        Assert( element );
        
        if ( element )
            node = new PLDoubleLinkedListNode<SimplePlayListElement>(element);
    
        Assert( node );
        
        if ( node )
        {   
            PLDoubleLinkedListNode<SimplePlayListElement>* deadNode;
            
            deadNode = this->AddToList(node);
            
            delete deadNode;
            addedSuccesfully = true;
        
        }
        
    }
    
    return addedSuccesfully;
}

