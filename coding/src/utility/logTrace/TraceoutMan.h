#pragma once

namespace TraceoutNM {
	typedef enum { TR_QUEUED = 0, TR_OUTPUT = 1, TR_DISCARD = 2 } TR_FLAG;
}

class TraceoutMan
{
public:
	static void OutputString( const char* text, TraceoutNM::TR_FLAG flag = TraceoutNM::TR_QUEUED, int deltaDepth = 0 );
};
