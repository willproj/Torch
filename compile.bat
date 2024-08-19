@echo off
setlocal

:: Set paths to glslc and shader files using absolute paths
set GLSLC=C:/VulkanSDK/1.3.290.0/Bin/glslc.exe
set SHADER_DIR=%~dp0assets\shader
set OUT_DIR=%~dp0assets\compiled_shaders

:: Create output directory if it doesn't exist
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

:: Function to compile shader if the source has changed
call :compile_shader "%SHADER_DIR%\shader.vert" "%OUT_DIR%\vert.spv"
call :compile_shader "%SHADER_DIR%\shader.frag" "%OUT_DIR%\frag.spv"

pause
exit /b

:compile_shader
set SRC_FILE=%~1
set OUT_FILE=%~2

:: Check if the output file needs to be updated
if not exist "%OUT_FILE%" (
    echo Compiling %SRC_FILE%...
    "%GLSLC%" "%SRC_FILE%" -o "%OUT_FILE%"
) else (
    for %%F in ("%SRC_FILE%") do set SRC_MODIFIED=%%~tF
    for %%F in ("%OUT_FILE%") do set OUT_MODIFIED=%%~tF

    if "%SRC_MODIFIED%" gtr "%OUT_MODIFIED%" (
        echo Recompiling %SRC_FILE%...
        "%GLSLC%" "%SRC_FILE%" -o "%OUT_FILE%"
    ) else (
        echo %OUT_FILE% is up to date.
    )
)
exit /b
