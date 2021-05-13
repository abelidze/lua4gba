@echo off
if not exist bin mkdir bin || goto :error
echo Compiling tools...
gcc -Wall -O2 -s gbfs.c djbasename.c -o .\bin\gbfs.exe -I..\include || goto :error
gcc -Wall -O2 -s lsgbfs.c -o .\bin\lsgbfs.exe -I..\include || goto :error
gcc -Wall -O2 -s ungbfs.c -o .\bin\ungbfs.exe -I..\include || goto :error
gcc -Wall -O2 -s padbin.c -o .\bin\padbin.exe -I..\include || goto :error
echo Done!
goto :end

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%

:end
