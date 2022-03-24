echo off

echo -----
echo Backing up old config, qtusers, and qtgroups files
move "c:\Program Files\VideoRelayServer\qtusers" "c:\Program Files\VideoRelayServer\qtusers.backup"
move "c:\Program Files\VideoRelayServer\qtgroups" "c:\Program Files\VideoRelayServer\qtgroups.backup"
move "c:\Program Files\VideoRelayServer\streamingserver.xml" "c:\Program Files\VideoRelayServer\streamingserver.xml.backup"
move "c:\Program Files\VideoRelayServer\relayconfig.xml" "c:\Program Files\VideoRelayServer\relayconfig.xml.backup"
move "c:\Program Files\VideoRelayServer\streamingadminserver.pem" "c:\Program Files\VideoRelayServer\streamingadminserver.pem.backup"
move "c:\Program Files\VideoRelayServer\streamingadminserver.conf" "c:\Program Files\VideoRelayServer\streamingadminserver.conf.backup"
move "c:\Program Files\VideoRelayServer\streamingloadtool.cfg" "c:\Program Files\VideoRelayServer\streamingloadtool.cfg.backup"

echo -----
echo Creating c:\Program Files\VideoRelayServer\ directory
mkdir "c:\Program Files\VideoRelayServer\"

echo -----
echo Copying files
copy VideoRelayServer.exe "c:\Program Files\VideoRelayServer\VideoRelayServer.exe"
copy streamingserver.xml "c:\Program Files\VideoRelayServer\streamingserver.xml-sample"
REM copy qtusers "c:\Program Files\VideoRelayServer\qtusers"
REM copy qtgroups "c:\Program Files\VideoRelayServer\qtgroups"
copy relayconfig.xml-Sample  "c:\Program Files\VideoRelayServer\relayconfig.xml-Sample"
copy PlaylistBroadcaster.exe  "c:\Program Files\VideoRelayServer\PlaylistBroadcaster.exe"
copy MP3Broadcaster.exe  "c:\Program Files\VideoRelayServer\MP3Broadcaster.exe"
copy streamingadminserver.pl "c:\Program Files\VideoRelayServer\streamingadminserver.pl"
REM copy streamingadminserver.pem "c:\Program Files\VideoRelayServer\streamingadminserver.pem"
copy qtpasswd.exe  "c:\Program Files\VideoRelayServer\qtpasswd.exe"
copy StreamingLoadTool.exe  "c:\Program Files\VideoRelayServer\StreamingLoadTool.exe"
copy streamingloadtool.cfg  "c:\Program Files\VideoRelayServer\streamingloadtool.cfg"
copy WinPasswdAssistant.pl "c:\Program Files\VideoRelayServer\WinPasswdAssistant.pl"

mkdir  "c:\Program Files\VideoRelayServer\QTSSModules\"
copy QTSSRefMovieModule.dll "c:\Program Files\VideoRelayServer\QTSSModules\QTSSRefMovieModule.dll"
REM copy QTSSRawFileModule.dll "c:\Program Files\VideoRelayServer\QTSSModules\QTSSRawFileModule.dll"
REM copy QTSSSpamDefenseModule.dll "c:\Program Files\VideoRelayServer\QTSSModules\QTSSSpamDefenseModule.dll"

mkdir  "c:\Program Files\VideoRelayServer\Playlists\"

mkdir  "c:\Program Files\VideoRelayServer\Movies\"
copy sample_100kbit.mov "c:\Program Files\VideoRelayServer\Movies\sample_100kbit.mov"
copy sample_300kbit.mov "c:\Program Files\VideoRelayServer\Movies\sample_300kbit.mov"
copy sample_100kbit.mp4 "c:\Program Files\VideoRelayServer\Movies\sample_100kbit.mp4"
copy sample_300kbit.mp4 "c:\Program Files\VideoRelayServer\Movies\sample_300kbit.mp4"
copy sample.mp3 "c:\Program Files\VideoRelayServer\Movies\sample.mp3"
copy sample_50kbit.3gp "c:\Program Files\VideoRelayServer\Movies\sample_50kbit.3gp"
copy sample_h264_1mbit.mp4 "c:\Program Files\VideoRelayServer\Movies\sample_h264_1mbit.mp4"
copy sample_h264_100kbit.mp4 "c:\Program Files\VideoRelayServer\Movies\sample_h264_100kbit.mp4"
copy sample_h264_300kbit.mp4 "c:\Program Files\VideoRelayServer\Movies\sample_h264_300kbit.mp4"

mkdir  "c:\Program Files\VideoRelayServer\Logs\"

rmdir /S /Q "c:\Program Files\VideoRelayServer\Documentation"
mkdir "c:\Program Files\VideoRelayServer\Documentation\"
copy readme.pdf "c:\Program Files\VideoRelayServer\Documentation"

rmdir /S /Q "c:\Program Files\VideoRelayServer\AdminHtml"
mkdir "c:\Program Files\VideoRelayServer\AdminHtml\"
copy AdminHtml\*.pl "c:\Program Files\VideoRelayServer\AdminHtml"
copy AdminHtml\*.cgi "c:\Program Files\VideoRelayServer\AdminHtml"
copy AdminHtml\*.html "c:\Program Files\VideoRelayServer\AdminHtml"

mkdir "c:\Program Files\VideoRelayServer\AdminHtml\images\"
copy AdminHtml\images\*.gif "c:\Program Files\VideoRelayServer\AdminHtml\images"

mkdir "c:\Program Files\VideoRelayServer\AdminHtml\includes\"
copy AdminHtml\includes\*.js "c:\Program Files\VideoRelayServer\AdminHtml\includes"

mkdir "c:\Program Files\VideoRelayServer\AdminHtml\html_en\"
copy AdminHtml\html_en\* "c:\Program Files\VideoRelayServer\AdminHtml\html_en"

"c:\Program Files\VideoRelayServer\DarwinStreamingServer" -i

echo -----
echo Adding the install dir to the system path
RegistrySystemPathEditor.exe

echo -----
perl "c:\Program Files\VideoRelayServer\WinPasswdAssistant.pl"
cls

echo -----------------------------------------------------------------
echo The VideoRelayServer has been installed.
echo -----------------------------------------------------------------
echo Launching the web based Streaming Admin Server
echo *** Closing this window will stop the Streaming Admin Server. ***
perl "c:\Program Files\VideoRelayServer\streamingadminserver.pl"
echo -----------------------------------------------------------------
echo Perl is not properly installed on your system.
echo This installer requires Perl to run the Streaming Admin Server.
echo:
echo If you wish to use the web admin user interface to configure Darwin 
echo Streaming Server, you will need to perform the following steps.
echo:
echo See the readme file on installing Perl.
echo Please make sure the path to the Perl interpreter is in your system path 
echo and run 
echo:
echo c:\Program Files\VideoRelayServer\streamingadminserver.pl
echo:
echo from the command prompt.

pause
