@echo off

if [%1]==[] goto usage

jlink -device %1 -CommanderScript jlink-prog-info0.txt
pause
goto :eof

:usage
@echo Usage: %0 ^<Chip Part#^>
exit /B 1
