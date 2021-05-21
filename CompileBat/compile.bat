echo off
cls

if exist *.exe del *.exe
if exist *.dll del *.dll
cl "./Source.cpp" kernel32.lib user32.lib gdi32.lib advapi32.lib
cl /LD "./ScreenHeight.cpp" kernel32.lib user32.lib gdi32.lib advapi32.lib
cl /LD "./SSE3Status.cpp" kernel32.lib user32.lib gdi32.lib advapi32.lib
del *.obj *.lib *.exp

pause