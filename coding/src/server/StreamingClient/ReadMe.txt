���Ź���

	1. Adaptor.h:  ��������
		a. ϵͳ��̨��
		b. CreateDVR==>theDVR
	2. RelayDVR.h:  CreateDVR����ClientSession���󣬸ö����IChannel�̳� ==>theChan
	3. IChannel->Play
	
ֹͣ����

	1. theChan->Stop
	2. theDVR->DestroyChannel
	3. delete theDVR
