rem -- 删除Visual C++的临时文件
del /F /S /Q  *.ncb
del /F /S /Q  *.opt
del /F /S /Q  *.aps
del /F /S /Q  *.clw
del /F /S /Q  *.plg
del /F /s /Q  *.scc
del /A:H /F /s /Q  *.scc
del /F /s /Q  *.cvsignore
del /F /s /Q  *.vcproj.vspscc
@
del /F /s /Q  *.suo
del /A:H /F /s /Q  *.suo
@
del /F /s /Q  *.user
@
del /F /s /Q /A:H .svn
@
del /F /s /Q /A:H 复件*
