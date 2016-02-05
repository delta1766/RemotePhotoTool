REM
REM RemotePhotoTool - remote camera control software
REM Copyright (C) 2008-2016 Michael Fink
REM
REM runs CppCheck to check sourcecode
REM

set PATH=%PATH%;"C:\Program Files\Cppcheck\"

REM when started via command line, assume "no xml" and "current folder"
set INTDIR=%1
if "%INTDIR%" == "" set INTDIR=%CD%\

REM when xml is passed as second param, set format and output file
set FORMAT=
set OUTFILE=%INTDIR%cppcheck.txt
if "%2" == "xml" set FORMAT=--xml
if "%2" == "xml" set OUTFILE=%INTDIR%cppcheck-Results.xml

REM run cppcheck
REM -I <dir>            Include path
REM -i <dir>            Ignore path
REM --suppressions-list=<file>   File with suppressed warnings
REM -j 4                Multithreading
REM --platform=win32W   Platform specific types
REM --language=c++      Language (file extensions)
REM --std=c++11         Language (syntax)%
REM --enable=all        Enable warnings
REM --template vs       Output format for warnings
REM --check-config
cppcheck.exe ..\Base\ ..\CanonControl\ ..\RemotePhotoTool\ ..\LuaScripting\ ..\RemotePhotoToolCmdline\ ..\RemoteScriptingEditor\ -I ..\Base\ -I ..\LuaScripting\ -I ..\CanonControl\exports\ -i ..\Thirdparty -i ..\LuaScripting\lua-5.3.2 -DWIN32 -D_WINDOWS -DNDEBUG -D__cplusplus --suppressions-list=cppcheck-suppress.txt -j 4 --platform=win32W --language=c++ --std=c++11 %FORMAT% --enable=all --template vs 2> %OUTFILE%
