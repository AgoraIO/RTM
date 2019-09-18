@echo off
echo rtm-build-release.bat can auto compile AgoraRTMTutorial windows solutions.
echo ==========================================================
SETLOCAL

del *.zip
del AgoraRTMTutorial_protable\**

::%1 param: 32 means x86,others means x64;
::%2 param: solution name. default AgoraRTMTutorial.sln
::%3 param: compile configuration examples: debug\release
::%4 param: compile tools .default:2013
::%5 param: compile sdk download url.
::%6 param: language 0 chinese;1 english
::%7 param: app branch.
IF "%1"=="32" (set Machine=x86) else (set Machine=x64)
IF "%4"=="2017" (set platformTool=2017) else (set platformTool=2013)
set ProjName=%2
set Config=%3
set rtcsdk_rul=%5
set language=%6
set demo_branch=%7
echo Machine: %Machine%
echo ProjName: %ProjName%
echo Config: %Config%
echo platformTool: %platformTool%
echo sdk_rul:%rtcsdk_rul%
echo language: %language%
echo demo_branch:%demo_branch%

set Local_Path=%~dp0%
echo LocalPath: %Local_Path%
cd %Local_Path%

::copy sdk files native.
::x86 need create lib_x64; x64 need create lib_x86
xcopy /y ..\products_sdk\sdk\include\*.h  .\sdk\include\*.h
xcopy /y ..\products_sdk\sdk\lib\*.lib    .\sdk\lib\*.lib
xcopy /y ..\products_sdk\sdk\dll\*.dll  .\sdk\dll\*.dll

::msbuild windows sln

if %platformTool%==2017 (
    call "%VS141COMNTOOLS%VsDevCmd.bat"
) else if %platformTool%==2013 (
    call "%VS120COMNTOOLS%VsDevCmd.bat"
)

if %Machine% == x64 (
    echo "vs%platformTool%-x64 compile ============================"
    msbuild %ProjName% /t:Rebuild /p:Configuration=%Config% /p:Platform=%Machine%
    IF NOT exist x64\release\AgoraRTMTutorial.exe (
        echo ##### msbuild AgoraRTMTutorial.sln /t:Rebuild /p:Configuration=Release;Platform=x64 #####
        echo ##### Failed! Error code is %ERRORLEVEL% #####
        EXIT /b %ERRORLEVEL%
    )
) else if %Machine% == x86 (
    echo "vs%platformTool%-x86 compile ============================"
    msbuild %ProjName% /t:Rebuild /p:Configuration=%Config% /p:Platform="Win32" /p:SubsystemVersion=5.1
    IF NOT exist release\AgoraRTMTutorial.exe (
        echo ##### msbuild AgoraRTMTutorial.sln /t:Rebuild /p:Configuration=Release;Platform=x86 #####
        echo ##### Failed! Error code is %ERRORLEVEL% #####
        EXIT /b %ERRORLEVEL%
    )
)

::zip output files include pdb
mkdir AgoraRTMTutorial_protable

copy /y .\sdk\dll\*.dll AgoraRTMTutorial_protable\*.dll
if %Machine% == x86 (
    xcopy /y release\*.exe AgoraRTMTutorial_protable\*.exe
    xcopy /y release\*.pdb AgoraRTMTutorial_protable\*.pdb
) else if %Machine% == x64 (
    xcopy /y x64\release\*.exe AgoraRTMTutorial_protable\*.exe
    xcopy /y x64\release\*.pdb AgoraRTMTutorial_protable\*.pdb
)

if %language%==0 (
    goto chinese
) else (
    goto english
)

:chinese
if "%time:~0,2%" lss "10" (set hh=0%time:~1,1%) else (set hh=%time:~0,2%)
echo hh:%hh%
set cdata=%date:~0,4%%date:~5,2%%date:~8,2%_%hh%%time:~3,2%%time:~6,2%
goto compileopt

:english
if "%time:~0,2%" lss "10" (set hh=0%time:~1,1%) else (set hh=%time:~0,2%)
echo hh:%hh%
set cdata=%date:~6,4%%date:~0,2%%date:~3,2%_%hh%%time:~3,2%%time:~6,2%
goto compileopt

:compileopt
echo %cdata%
if %rtcsdk_rul%==0 (
    goto sdkcompile
) else (
    goto urlcompile
)

:sdkcompile
set build_no=
echo "sdkcompile build_no: " %build_no%
7z.exe a -tzip AgoraRTMTutorial_for_windows(%Machine%)_%mediaType%_v%demo_branch%_%cdata%.zip AgoraRTMTutorial_protable
echo sdk=AgoraRTMTutorial_for_windows(%Machine%)_%mediaType%_v%demo_branch%_%cdata%.zip > propsfile
exit

:urlcompile
set build_no=%rtcsdk_rul:~-8,4%
echo "urlcompile build_no: " %build_no%
7z.exe a -tzip AgoraRTMTutorial_for_windows(%Machine%)_%mediaType%_v%demo_branch%_%cdata%_%build_no%.zip AgoraRTMTutorial_protable
echo sdk=AgoraRTMTutorial_for_windows(%Machine%)_%mediaType%_v%demo_branch%_%cdata%_%build_no%.zip > propsfile
exit