set QT5_DIR=%QT5DIR%
set CURRENT_DIR=%~dp0
set BUILD_NAME=build

if not exist %CURRENT_DIR%%BUILD_NAME% (
    mkdir %CURRENT_DIR%%BUILD_NAME%
)
cd %CURRENT_DIR%%BUILD_NAME%

cmake .. -G "Visual Studio 15 2017 Win64"

pause