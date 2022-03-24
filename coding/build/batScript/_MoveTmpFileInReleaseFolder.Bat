rem -- 删除Visual C++的临时文件
del /F /S /Q  *.ncb
del /F /S /Q  *.opt
del /F /S /Q  *.aps
del /F /S /Q  *.clw
del /F /S /Q  *.plg
@rem ==
del /F /S /Q  *.bsc
del /F /S /Q  *.pdb
del /F /S /Q  *.ilk
del /F /S /Q  *.SUP
@rem ==
del /F /S /Q  Darwi*.*
del /F /S /Q  *CPPUnit*.*
del /F /S /Q  *复件*.*
@rem ==删除中间文件
del /F /S /Q  APICommonCode*.*
del /F /S /Q  APIStubLib*.*
del /F /S /Q  CommonUtilities*.*
del /F /S /Q  QTSSRawFile*.*
del /F /S /Q  SafeDynamicModule*.*
del /F /S /Q  SafeInternalStdLib*.*
del /F /S /Q  QTSSSpamDefense*.*
del /F /S /Q  RelayServerDVRAdaptor*.exp
del /F /S /Q  RelayServerDVRAdaptor*.lib
del /F /S /Q  QTSSRefMovieModule*.*
del /F /S /Q  QTFileLib*.*
@rem ==需确定是否删除文件
del /F /S /Q  *.lib
del /F /S /Q  *.exp
del /F /S /Q  registry*.exe
del /F /S /Q  *.map
del /F /S /Q  vss*.scc