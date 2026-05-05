@echo off
setlocal

set "PROJECT=rpg075_fix"
set "OUT=%PROJECT%.gb"
set "SRC=main.c sprites.c dialogue.c jpfont.c misakiUTF16.c messages_bank.c messages_runtime.c banked_graphics.c map_data_bank.c map_data_runtime.c font_data_bank.c font_data_runtime.c battle_data_bank.c battle_data_runtime.c"
set "ROM_BANKS=8"
set "CART_TYPE=0x19"
set "LOG=build.log"
set "PRECHECK_LOG=precheck_output.log"
set "COMPILE_LOG=compile_output.log"
set "ROMUSAGE_LOG=romusage_output.log"
set "DANGER_LOG=dangerous_warnings.log"
set "ROMUSAGE_DANGER=romusage_danger.log"

if defined GBDK_HOME (
    if exist "%GBDK_HOME%\bin\lcc.exe" (
        set "LCC=%GBDK_HOME%\bin\lcc.exe"
        goto :FIND_ROMUSAGE
    )
)

if defined GBDK (
    if exist "%GBDK%\bin\lcc.exe" (
        set "LCC=%GBDK%\bin\lcc.exe"
        goto :FIND_ROMUSAGE
    )
)

where lcc.exe >nul 2>&1
if %ERRORLEVEL%==0 (
    set "LCC=lcc.exe"
    goto :FIND_ROMUSAGE
)

echo [ERROR] lcc.exe was not found.
pause
exit /b 1

:FIND_ROMUSAGE
set "ROMUSAGE="
if defined GBDK_HOME (
    if exist "%GBDK_HOME%\bin\romusage.exe" set "ROMUSAGE=%GBDK_HOME%\bin\romusage.exe"
)
if not defined ROMUSAGE if defined GBDK (
    if exist "%GBDK%\bin\romusage.exe" set "ROMUSAGE=%GBDK%\bin\romusage.exe"
)
if not defined ROMUSAGE (
    where romusage.exe >nul 2>&1
    if %ERRORLEVEL%==0 set "ROMUSAGE=romusage.exe"
)

del "%LOG%" "%PRECHECK_LOG%" "%COMPILE_LOG%" "%ROMUSAGE_LOG%" "%DANGER_LOG%" "%ROMUSAGE_DANGER%" >nul 2>&1

echo Building %OUT% ... > "%LOG%"
echo Sources: %SRC% >> "%LOG%"
echo. >> "%LOG%"

where python >nul 2>&1
if %ERRORLEVEL%==0 (
    if exist "tools\prebank_check.py" (
        python tools\prebank_check.py > "%PRECHECK_LOG%" 2>&1
        type "%PRECHECK_LOG%" >> "%LOG%"
        echo. >> "%LOG%"
    )
)

echo Command: "%LCC%" -msm83:gb -Wl-m -Wl-j -Wl-yt%CART_TYPE% -Wl-yo%ROM_BANKS% -o "%OUT%" %SRC% >> "%LOG%"
echo. >> "%LOG%"

"%LCC%" -msm83:gb -Wl-m -Wl-j -Wl-yt%CART_TYPE% -Wl-yo%ROM_BANKS% -o "%OUT%" %SRC% > "%COMPILE_LOG%" 2>&1
set "BUILD_RC=%ERRORLEVEL%"
type "%COMPILE_LOG%" >> "%LOG%"

echo. >> "%LOG%"
if not "%BUILD_RC%"=="0" (
    echo [ERROR] Build failed: exit=%BUILD_RC% >> "%LOG%"
    type "%LOG%"
    pause
    exit /b %BUILD_RC%
)

echo [OK] Build finished: %OUT% >> "%LOG%"

echo. >> "%LOG%"
echo ===== dangerous linker warning check ===== >> "%LOG%"
findstr /i /c:"Write from one bank spans into the next" /c:"ASlink-Warning" /c:"Possible overflow" /c:"Multiple write" /c:"overflow" /c:"relocation truncated" /c:"segment overlap" "%COMPILE_LOG%" > "%DANGER_LOG%" 2>nul
if exist "%DANGER_LOG%" (
    for %%A in ("%DANGER_LOG%") do if %%~zA GTR 0 (
        type "%DANGER_LOG%" >> "%LOG%"
        echo [ERROR] Dangerous compiler/linker warning detected. Treating build as failed. >> "%LOG%"
        type "%LOG%"
        pause
        exit /b 1
    )
)
echo [OK] no dangerous compiler/linker warning keywords found >> "%LOG%"

echo. >> "%LOG%"
echo ===== romusage check ===== >> "%LOG%"
if defined ROMUSAGE (
    echo [INFO] romusage found: %ROMUSAGE% >> "%LOG%"
    if exist "%PROJECT%.map" (
        echo ---- romusage %PROJECT%.map -a -g -B ---- >> "%LOG%"
        "%ROMUSAGE%" "%PROJECT%.map" -a -g -B > "%ROMUSAGE_LOG%" 2>&1
        type "%ROMUSAGE_LOG%" >> "%LOG%"
        echo ---- romusage %PROJECT%.map -q -E ---- >> "%LOG%"
        "%ROMUSAGE%" "%PROJECT%.map" -q -E >> "%LOG%" 2>&1

        findstr /i /c:"Possible overflow beyond Bank 0" /c:"Possible overflow" /c:"Multiple write" /c:"Write from one bank spans into the next" "%ROMUSAGE_LOG%" > "%ROMUSAGE_DANGER%" 2>nul
        if exist "%ROMUSAGE_DANGER%" (
            for %%A in ("%ROMUSAGE_DANGER%") do if %%~zA GTR 0 (
                echo [ERROR] Dangerous romusage warning detected. Treating build as failed. >> "%LOG%"
                type "%ROMUSAGE_DANGER%" >> "%LOG%"
                type "%LOG%"
                pause
                exit /b 1
            )
        )
    ) else (
        echo [WARN] map file not found: %PROJECT%.map >> "%LOG%"
    )
) else (
    echo [WARN] romusage.exe was not found. >> "%LOG%"
)

echo. >> "%LOG%"
echo ===== generated files ===== >> "%LOG%"
dir *.gb *.map *.noi *.sym >> "%LOG%" 2>nul

type "%LOG%"
pause
exit /b 0
