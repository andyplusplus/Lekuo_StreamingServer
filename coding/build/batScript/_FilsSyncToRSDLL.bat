@rem - 
@rem - 该文件把系统运行的相关文件与  运行目录  中的文件进行同步
@rem -
@
@rem - 拷备配置文件到运行目录
xcopy /C /R /Y .\Config\VideoRelay.ini  D:\RSDLL\
xcopy /C /R /Y .\Config\*.*      D:\RSDLL\
@
@
@rem - 拷备库文件到运行目录
@
xcopy /C /R /Y .\Lib\HikDLL\*.dll      D:\RSDLL\
xcopy /C /R /Y .\Lib\HikLib\*.lib      D:\RSDLL\
@
@
@rem - TBD: 把输出的相关文件Copy到Output目录
@