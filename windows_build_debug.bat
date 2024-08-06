@echo off
setlocal

:: Define directories
set "SOURCE_DIR=%~dp0"  :: This should be the directory where the script is located
set "BUILD_DIR=%SOURCE_DIR%out\build"

:: Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

:: Navigate to build directory
pushd "%BUILD_DIR%"

:: Run CMake to generate build files
echo Generating build files with CMake...
cmake -DCMAKE_BUILD_TYPE=Debug "%SOURCE_DIR%"

:: Build the project using MSBuild
echo Building the project with MSBuild...
cmake --build . --config Debug

:: Optionally, you can also run the executable (uncomment the following line if needed)
:: echo Running the application...
:: "%BUILD_DIR%\bin\Torch.exe"

:: Return to the original directory
popd

echo Debug build and configuration completed.

endlocal
pause
