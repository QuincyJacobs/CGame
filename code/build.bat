@echo off

REM Use this as shortcut target to run at startup:
REM C:\Windows\System32\cmd.exe /k w:\CGame\misc\shell.bat

REM // TODO(Quincy): Can we just build x86 in this command?

IF NOT EXIST ..\build  mkdir ..\build
pushd ..\build
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4189 -wd4100 -DCGAME_INTERNAL=1 -DCGAME_SLOW=1 -DCGAME_WIN32=1 -FA -FC -Z7 -Fmwin32_cgame.map w:\CGame\code\win32_cgame.cpp /link -opt:ref -subsystem:windows,5.2 user32.lib gdi32.lib
popd
