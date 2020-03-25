@echo off

REM Use this as shortcut target to run at startup:
REM C:\Windows\System32\cmd.exe /k w:\CGame\misc\shell.bat

set CommonCompilerFlags= -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4189 -wd4100 -DCGAME_INTERNAL=1 -DCGAME_SLOW=1 -DCGAME_WIN32=1 -FA -FC -Z7 -Fmwin32_cgame.map
set CommonLinkerFlags= -opt:ref user32.lib gdi32.lib winmm.lib

REM // TODO(Quincy): Can we just build x86 in this command?

IF NOT EXIST ..\build  mkdir ..\build
pushd ..\build

REM 32-bit build
REM cl %CommonCompilerFlags% w:\CGame\code\win32_cgame.cpp /link -subsystem:windows,5.2 %CommonLinkerFlags%

REM 64-bit build
cl %CommonCompilerFlags% w:\CGame\code\win32_cgame.cpp /link %CommonLinkerFlags%

popd


