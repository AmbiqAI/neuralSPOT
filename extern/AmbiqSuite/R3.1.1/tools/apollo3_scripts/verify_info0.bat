@echo off

if [%1]==[] goto usage

jlink -device %1 -CommanderScript jlink-read-info0.txt
diff info0.bin info0_dump.bin
@ECHO OFF
if ERRORLEVEL 1 (
    echo "INFO0 Invalid"
) else (
    echo "INFO0 Valid"
)
pause
goto :eof
:usage
@echo Usage: %0 ^<Chip Part#^>
exit /B 1
