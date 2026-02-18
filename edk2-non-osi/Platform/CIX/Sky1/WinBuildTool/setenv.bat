@echo off

set MINGW_PATH=YOUR_MINGW_PATH
set ARM_GUN_PATH=YOUR_GUN_PATH
set PYTHON_PATH=YOUR_PYTHON_PATH

rem =========================
rem Local Path
rem =========================
set WORKSPACE=%CD%
set TOOLCHAIN_PATH=%CD%\Toolchain
rem =========================
rem workaround for echo command
rem =========================
set PATH=%PATH%;%TOOLCHAIN_PATH%
rem =========================
rem set GNU make
rem =========================
SET TOOLCHAIN=GCC5
set PATH=%PATH%;%MINGW_PATH%\bin;
set GCC5_AARCH64_PREFIX=%ARM_GUN_PATH%\bin\aarch64-none-linux-gnu-
rem =========================
rem set Python path
rem =========================
set PYTHON_COMMAND=%PYTHON_PATH%\python.exe
rem =========================
rem copy Base tools
rem =========================
if not exist %WORKSPACE%\edk2\BaseTools\Bin\Win32 (
xcopy %WORKSPACE%\Toolchain\BaseTools\ %WORKSPACE%\edk2\BaseTools\Bin\ /Y /F /S /E
)
rem =========================
rem  set packages paths
rem =========================
set PACKAGES_PATH=%WORKSPACE%\edk2;%WORKSPACE%\edk2-platforms;%WORKSPACE%\edk2-non-osi;

rem rd /s /Q Build

call edk2\edksetup.bat rebuild

rem rd /s /Q %WORKSPACE%\Bin
md  %WORKSPACE%\Bin


