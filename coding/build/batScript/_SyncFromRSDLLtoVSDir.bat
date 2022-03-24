@rem - 
@rem - 该文件把系统运行的相关文件与  运行目录  中的文件进行同步
@rem -
@
@rem - 拷备配置文件到运行目录
xcopy /C /R /Y            D:\RSDLL\RelayServerDVRAdaptor*.dll						"C:\Program Files\VideoRelayServer\"
@
@
@rem - 拷备库文件到运行目录
@
xcopy /C /R /Y           D:\RSDLL\VHikDVR*.dll														"C:\Program Files\VideoRelayServer\"
xcopy /C /R /Y           D:\RSDLL\VideoRelayServer*.exe									"C:\Program Files\VideoRelayServer\"
xcopy /C /R /Y           D:\RSDLL\QTSSTestDevModuleD.dll									"C:\Program Files\VideoRelayServer\QTSSModules\"
xcopy /C /R /Y           D:\RSDLL\VideoRelay.ini													"C:\Program Files\VideoRelayServer\"
@
@
@rem - TBD: 把输出的相关文件Copy到Output目录
@