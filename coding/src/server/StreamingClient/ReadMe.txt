播放过程

	1. Adaptor.h:  是主程序
		a. 系统初台化
		b. CreateDVR==>theDVR
	2. RelayDVR.h:  CreateDVR创建ClientSession对象，该对象从IChannel继承 ==>theChan
	3. IChannel->Play
	
停止过程

	1. theChan->Stop
	2. theDVR->DestroyChannel
	3. delete theDVR
