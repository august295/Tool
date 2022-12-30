set CURRENT_DIR=%~dp0
set BUILD_NAME=build

if not exist %CURRENT_DIR%%BUILD_NAME% (
    mkdir %CURRENT_DIR%%BUILD_NAME%
)
cd %CURRENT_DIR%%BUILD_NAME%

cmake .. -G "Visual Studio 17 2022" -A x64

pause