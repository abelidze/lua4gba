@echo off
if not exist ..\tools\bin cd ..\tools && call mktools.bat && cd .. || goto :error
echo Adding source files to ROM...
..\tools\bin\padbin 0x100 ..\lua4gba.gba || goto :error
..\tools\bin\gbfs demo.gbfs demo.lua || goto :error
copy /b ..\lua4gba.gba + demo.gbfs demo.gba || goto :error
del demo.gbfs || goto :error
echo Done! [ demo.gba ] created.
goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
