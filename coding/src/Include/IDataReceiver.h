//#include "IDataReceiver.h"

#if !defined(LK_I_DataSendReceiver_H_INCLUDED)
#define LK_I_DataSendReceiver_H_INCLUDED

#include "LeKuo.h"

class IDataReceiver
{
public:
	virtual unsigned long ReceiveData(	HANDLE hSender, Tag inSenderTag,
		unsigned char* inData, 	unsigned long inLength, unsigned long inDataType, unsigned long* outReceiveResult=NULL) = 0;

	virtual ~IDataReceiver() {};
	virtual LK_Error Open(unsigned char* /*inData*/, unsigned long /*inLength*/) {return LK_NoErr;};
	virtual LK_Error Close() {return LK_NoErr;};
};

typedef IDataReceiver* LPIDataReceiver;


#endif // !defined(LK_I_DataSendReceiver_H_INCLUDED)
