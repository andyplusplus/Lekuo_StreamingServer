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
//---------向播放器加数据,马上返回.  返回值1:加成功,0:不成功 数据缓冲满,-1:异常.
BOOL WINAPI MP4_AddBuffer_Wait(LPBYTE lpBuffer,DWORD dwBytes,int WatchID,DWORD dwWaitTime);//dwBytes<32768
//---------向播放器加数据,若数据缓冲满,最多等待dwWaitTime毫秒. 返回值1:加成功,0:不成功 数据缓冲满,-1:异常.

BOOL WINAPI MP4_AddMotionBuffer(UINT32 * lpBuffer,DWORD dwBytes,int WatchID,BOOL bMotionDetect);

void WINAPI MP4_ReadBuffer(LPBYTE lpBuffer,DWORD *dwBytes,int WatchID);

BOOL WINAPI MP4_Open(int WatchID,HWND hWnd,COLORREF ColorKey,DWORD dwWidth,
					 DWORD  dwHeight,int Frame_Rate,BOOL bPlayFile=FALSE);
//---------打开播放器,创建解码线程,WathID必须唯一,其他所有相同WatchID函数在此执行之后调用. bPlayFile=TRUE:保持恒速解码,FALSE:根据缓冲动态调节
BOOL WINAPI MP4_Open_ext(int WatchID,HWND hWnd,COLORREF ColorKey,DWORD dwWidth,DWORD  dwHeight,
						 int Frame_Rate,BOOL bPlayFile,int avcodeType);
//---------其他跟MP4_Open相同，avcodeType(视频类型) 0:视通,视新MPEG4, 1:VWIS MPEG4;

void WINAPI MP4_Close(int WatchID);
//---------关闭播放器,结束解码线程,释放内存.WatchID跟MP4_Open配套,其他所有相同WatchID函数在此执行之后不能调用，bPlayFile=TRUE,保持恒速解码

void WINAPI MP4_CloseAll();

BOOL WINAPI MP4_SoundOpen(int WatchID);
//---------打开声音

BOOL WINAPI MP4_SoundClose(int WatchID);
//---------关闭声音

void WINAPI MP4_WriteBMP(int WatchID,char* Filename);
//---------抓位图

void WINAPI MP4_ColorAdjust(int WatchID,int Lum_Gain,int Lum_Offset,int hue,int sat,BOOL bFilter_Falg);
/*	LumGain;				//0....256				default:128
	LumOffset;				//-256...256			default:0
	hue;				//-180 ... 0 ... 180	default:0
	sat ;	//0 (BW) - 64 (normal) - 128 (too much color);	default:64
*/
//-------调节色彩--LumGain:对比度,Lum_Offset:亮度,hue:色度,sat:饱和度(bFilter_Falg=1时起作用)

void WINAPI MP4_ShowInfo(int WatchID,char* Info,BOOL bShow);
//---------bShow=1:左上角显示OSD    bShow=2:左上角显示OSD,后跟GOP时间

void WINAPI MP4_ShowInfo_Ex(int WatchID,int x,int y,char* Info,BOOL bShow);
//---------显示窗口叠加方式显示OSD,不随窗口大小变化而变化 
//bShowOSD=0:不显示OSD -- bShow=1:显示OSD，   -- bShow=2:显示OSD,后跟GOP时间
//x:横坐标开始位置
//y:纵坐标开始位置
//-----------
void WINAPI MP4_ShowOSD(int WatchID,int x,int y,char* Info,BOOL bShowOSD);
//---------图象合成方式显示OSD,随窗口大小变化而变化
//bShowOSD=0:不显示OSD -- bShow=1:显示OSD，  -- bShow=2:显示OSD,后跟GOP时间 
//x:横坐标开始位置
//y:纵坐标开始位置
//-----------

BOOL WINAPI MP4_WriteAVI(int WatchID,char* Filename);
//---------播放数据另保存为AVI文件

BOOL WINAPI MP4_CloseWriteAVI(int WatchID);
//---------结束保存AVI文件

BOOL WINAPI MP4_SetQuality(int WatchID,int postproc_level);
//---------设置回放画质postproc_level=0-6, 0:画质最差,速度最快  6:速度最慢,画质最好

void WINAPI MP4_ChangePos();

BOOL WINAPI MP4_ChangeVidStandard(int WatchID ,DWORD dwWidth,DWORD  dwHeight,int Frame_Rate,BOOL bPlayFile);
//---------改变分辨率, bPlayFile=TRUE:保持恒速解码,bPlayFile=FALSE根据缓冲动态调节

BOOL WINAPI MP4_GetGopTimeCode(int WatchID,int *pHour,int *pMinute,int *pSecond,int *pFrame);
//---------获取视频流数据时间

BOOL WINAPI MP4_ReFindIframe(int WatchID);
//---------重新定位I帧,使画面从I帧开始回放

BOOL WINAPI MP4_GetWaterMarkInfo(int WatchID,int *strInfo);
//---------获取水印信息，返回值 0:无水印  1:有水印  -1:还没开始检测, strinfo:水印信息,长度最大255字节

BOOL WINAPI MP4_ResetBuffer(int WatchID);
//---------重置解码缓冲区为开始状态(无数据状态)

int WINAPI MP4_GetBufferSize(int WatchID);
//---------获取正在解码的数据缓冲的数目

BOOL WINAPI MP4_ChangeFrame_Rate(int WatchID ,int Frame_Rate);
//---------改变播放帧率

int WINAPI MP4_GetCurFrameNum(int WatchID);
//---------获取正在解码数据的帧号

int WINAPI MP4_GetCurTime(int WatchID);
//---------获取正在解码数据的时间(秒)

BOOL WINAPI MP4_Deinterlace(int WatchID,int deinterlace);
//---------设置回放解交错deinterlace=0, 0:不解交错  1:BlendLinear方式解交错  2:InterpolateCubic方式解交错

BOOL WINAPI MP4_Throw_B_Frame(int WatchID,int num);
//num:丢B帧的数目 0:不丢; 1:丢1个; 2:丢2个

BOOL WINAPI MP4_Throw_PB_Frame(int WatchID,BOOL bThrow);
//bThrow:1只解I帧; 0:IP帧正常解


//以上函数最好不要多线程并行执行，要串行执行

#endif //_MP4__H