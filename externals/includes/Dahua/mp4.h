/*
**	******************************************************************************************
**
**                                 DH-MPEG4 Decode Library
**
**         (c) Copyright 1992-2003, ZheJiang Dahua Infomation Technology Stock CO.LTD.
**                                    All Rights Reserved
**
**	File		: mp4.h
**	Description	: 2004-3-17
**	By			: DAHUA_UI_GROUP
**	******************************************************************************************
*/

#ifndef _MP4__H
#define _MP4__H

BOOL WINAPI MP4_Init();
void WINAPI MP4_Play(int WatchID);

BOOL WINAPI MP4_AddBuffer(LPBYTE lpBuffer,DWORD dwBytes,int WatchID);//dwBytes<32768
//---------�򲥷���������,���Ϸ���.  ����ֵ1:�ӳɹ�,0:���ɹ� ���ݻ�����,-1:�쳣.
BOOL WINAPI MP4_AddBuffer_Wait(LPBYTE lpBuffer,DWORD dwBytes,int WatchID,DWORD dwWaitTime);//dwBytes<32768
//---------�򲥷���������,�����ݻ�����,���ȴ�dwWaitTime����. ����ֵ1:�ӳɹ�,0:���ɹ� ���ݻ�����,-1:�쳣.

BOOL WINAPI MP4_AddMotionBuffer(UINT32 * lpBuffer,DWORD dwBytes,int WatchID,BOOL bMotionDetect);

void WINAPI MP4_ReadBuffer(LPBYTE lpBuffer,DWORD *dwBytes,int WatchID);

BOOL WINAPI MP4_Open(int WatchID,HWND hWnd,COLORREF ColorKey,DWORD dwWidth,
					 DWORD  dwHeight,int Frame_Rate,BOOL bPlayFile=FALSE);
//---------�򿪲�����,���������߳�,WathID����Ψһ,����������ͬWatchID�����ڴ�ִ��֮�����. bPlayFile=TRUE:���ֺ��ٽ���,FALSE:���ݻ��嶯̬����
BOOL WINAPI MP4_Open_ext(int WatchID,HWND hWnd,COLORREF ColorKey,DWORD dwWidth,DWORD  dwHeight,
						 int Frame_Rate,BOOL bPlayFile,int avcodeType);
//---------������MP4_Open��ͬ��avcodeType(��Ƶ����) 0:��ͨ,����MPEG4, 1:VWIS MPEG4;

void WINAPI MP4_Close(int WatchID);
//---------�رղ�����,���������߳�,�ͷ��ڴ�.WatchID��MP4_Open����,����������ͬWatchID�����ڴ�ִ��֮���ܵ��ã�bPlayFile=TRUE,���ֺ��ٽ���

void WINAPI MP4_CloseAll();

BOOL WINAPI MP4_SoundOpen(int WatchID);
//---------������

BOOL WINAPI MP4_SoundClose(int WatchID);
//---------�ر�����

void WINAPI MP4_WriteBMP(int WatchID,char* Filename);
//---------ץλͼ

void WINAPI MP4_ColorAdjust(int WatchID,int Lum_Gain,int Lum_Offset,int hue,int sat,BOOL bFilter_Falg);
/*	LumGain;				//0....256				default:128
	LumOffset;				//-256...256			default:0
	hue;				//-180 ... 0 ... 180	default:0
	sat ;	//0 (BW) - 64 (normal) - 128 (too much color);	default:64
*/
//-------����ɫ��--LumGain:�Աȶ�,Lum_Offset:����,hue:ɫ��,sat:���Ͷ�(bFilter_Falg=1ʱ������)

void WINAPI MP4_ShowInfo(int WatchID,char* Info,BOOL bShow);
//---------bShow=1:���Ͻ���ʾOSD    bShow=2:���Ͻ���ʾOSD,���GOPʱ��

void WINAPI MP4_ShowInfo_Ex(int WatchID,int x,int y,char* Info,BOOL bShow);
//---------��ʾ���ڵ��ӷ�ʽ��ʾOSD,���洰�ڴ�С�仯���仯 
//bShowOSD=0:����ʾOSD -- bShow=1:��ʾOSD��   -- bShow=2:��ʾOSD,���GOPʱ��
//x:�����꿪ʼλ��
//y:�����꿪ʼλ��
//-----------
void WINAPI MP4_ShowOSD(int WatchID,int x,int y,char* Info,BOOL bShowOSD);
//---------ͼ��ϳɷ�ʽ��ʾOSD,�洰�ڴ�С�仯���仯
//bShowOSD=0:����ʾOSD -- bShow=1:��ʾOSD��  -- bShow=2:��ʾOSD,���GOPʱ�� 
//x:�����꿪ʼλ��
//y:�����꿪ʼλ��
//-----------

BOOL WINAPI MP4_WriteAVI(int WatchID,char* Filename);
//---------������������ΪAVI�ļ�

BOOL WINAPI MP4_CloseWriteAVI(int WatchID);
//---------��������AVI�ļ�

BOOL WINAPI MP4_SetQuality(int WatchID,int postproc_level);
//---------���ûطŻ���postproc_level=0-6, 0:�������,�ٶ����  6:�ٶ�����,�������

void WINAPI MP4_ChangePos();

BOOL WINAPI MP4_ChangeVidStandard(int WatchID ,DWORD dwWidth,DWORD  dwHeight,int Frame_Rate,BOOL bPlayFile);
//---------�ı�ֱ���, bPlayFile=TRUE:���ֺ��ٽ���,bPlayFile=FALSE���ݻ��嶯̬����

BOOL WINAPI MP4_GetGopTimeCode(int WatchID,int *pHour,int *pMinute,int *pSecond,int *pFrame);
//---------��ȡ��Ƶ������ʱ��

BOOL WINAPI MP4_ReFindIframe(int WatchID);
//---------���¶�λI֡,ʹ�����I֡��ʼ�ط�

BOOL WINAPI MP4_GetWaterMarkInfo(int WatchID,int *strInfo);
//---------��ȡˮӡ��Ϣ������ֵ 0:��ˮӡ  1:��ˮӡ  -1:��û��ʼ���, strinfo:ˮӡ��Ϣ,�������255�ֽ�

BOOL WINAPI MP4_ResetBuffer(int WatchID);
//---------���ý��뻺����Ϊ��ʼ״̬(������״̬)

int WINAPI MP4_GetBufferSize(int WatchID);
//---------��ȡ���ڽ�������ݻ������Ŀ

BOOL WINAPI MP4_ChangeFrame_Rate(int WatchID ,int Frame_Rate);
//---------�ı䲥��֡��

int WINAPI MP4_GetCurFrameNum(int WatchID);
//---------��ȡ���ڽ������ݵ�֡��

int WINAPI MP4_GetCurTime(int WatchID);
//---------��ȡ���ڽ������ݵ�ʱ��(��)

BOOL WINAPI MP4_Deinterlace(int WatchID,int deinterlace);
//---------���ûطŽ⽻��deinterlace=0, 0:���⽻��  1:BlendLinear��ʽ�⽻��  2:InterpolateCubic��ʽ�⽻��

BOOL WINAPI MP4_Throw_B_Frame(int WatchID,int num);
//num:��B֡����Ŀ 0:����; 1:��1��; 2:��2��

BOOL WINAPI MP4_Throw_PB_Frame(int WatchID,BOOL bThrow);
//bThrow:1ֻ��I֡; 0:IP֡������


//���Ϻ�����ò�Ҫ���̲߳���ִ�У�Ҫ����ִ��

#endif //_MP4__H