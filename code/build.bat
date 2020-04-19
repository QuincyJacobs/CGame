@echo off

REM Use this as shortcut target to run at startup:
REM C:\Windows\System32\cmd.exe /k w:\CGame\misc\shell.bat

set CommonCompilerFlags= -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4189 -wd4100 -DCGAME_INTERNAL=1 -DCGAME_SLOW=1 -DCGAME_WIN32=1 -FA -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

REM // TODO(Quincy): Can we just build x86 in this command?

IF NOT EXIST ..\build  mkdir ..\build
pushd ..\build

REM 32-bit build
REM cl %CommonCompilerFlags% -Fmwin32_cgame.map w:\CGame\code\win32_cgame.cpp /link -subsystem:windows,5.2 %CommonLinkerFlags%

REM 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% -Fmcgame.map w:\CGame\code\cgame.cpp -LD /link -incremental:no -PDB:cgame_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb -EXPORT:GameUpdateAndRender -EXPORT:GameGetSoundSamples
cl %CommonCompilerFlags% -Fmwin32_cgame.map w:\CGame\code\win32_cgame.cpp /link %CommonLinkerFlags%

popd


