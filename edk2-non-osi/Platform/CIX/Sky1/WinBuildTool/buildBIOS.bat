call setenv

@echo off
if "%1" == "clean" (
    rd /s /Q Build
    echo "Clean Success"
    goto :END
)

if "%1" == "" (
SET UEFI_TARGET=RELEASE
) else (
SET UEFI_TARGET=%1
)

if "%2" == "" (
SET PACKAGE_NAME=Merak
) else (
SET PACKAGE_NAME=%2
)

SET PACKAGE_TOOL_PATH=%WORKSPACE%\edk2-non-osi\Platform\CIX\Sky1\PackageTool
SET SAVE_BIOS_PATH=%WORKSPACE%\Bin
SET SAVE_BIOS_NAME=%PACKAGE_NAME%.bin
SET FD_NAME=SKY1_BL33_UEFI.fd

for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do call set UEFI_TARGET=%%UEFI_TARGET:%%i=%%i%%
xcopy %TOOLCHAIN_PATH%\PackageTool\ %PACKAGE_TOOL_PATH%\ /Y /F /S /E
::Get version
CD %WORKSPACE%\edk2-platforms
git log -1 --format="%%%H" >version.txt
SET /p PLATFORM_COMMIT_ID=<version.txt
del /f version.txt
::Get Date
for /f "tokens=1-3 delims=/- " %%1 in ("%date%") do set DATATIME=%%1%%2%%3
cd %WORKSPACE%
SET PLATFORM_COMMIT_ID=%PLATFORM_COMMIT_ID:~0,8%

call build -a AARCH64 -t %TOOLCHAIN% -p Platform\CIX\Sky1\%PACKAGE_NAME%\%PACKAGE_NAME%.dsc ^
-D BOARD_NAME=%PACKAGE_NAME% ^
-D COMMIT_HASH=%PLATFORM_COMMIT_ID% ^
-D COMPILE_BUILD_DATE=%DATATIME% ^
-D VARIABLE_TYPE=SPI ^
-D STANDARD_MM=TRUE ^
-D ACPI_BOOT_ENABLE=1 ^
-b %UEFI_TARGET% 

if %errorlevel% NEQ 0 goto :FAIL

cd %PACKAGE_TOOL_PATH%
copy /Y %WORKSPACE%\Build\%PACKAGE_NAME%\%UEFI_TARGET%_%TOOLCHAIN%\FV\%FD_NAME% %PACKAGE_TOOL_PATH%\Firmwares\%FD_NAME%
if %errorlevel% NEQ 0 goto :FAIL

fsutil file createNew Firmwares/dummy.bin 8192

cert_uefi_create_rsa.exe --key-alg rsa --key-size 3072 --hash-alg sha256 -p --ntfw-nvctr 223 ^
--nt-fw-cert certs\nt_fw_cert.crt ^
--nt-fw-key-cert certs\nt_fw_key.crt ^
--nt-fw-key Keys\oem_privatekey.pem ^
--non-trusted-world-key Keys\oem_privatekey.pem ^
--nt-fw Firmwares\%FD_NAME%

if %errorlevel% NEQ 0 goto :FAIL

fiptool.exe create ^
--trusted-key-cert certs\trusted_key_no.crt ^
--nt-fw-key-cert certs\nt_fw_key.crt ^
--nt-fw-cert certs\nt_fw_cert.crt ^
--nt-fw Firmwares\%FD_NAME% ^
Firmwares\bootloader3.img

if %errorlevel% NEQ 0 goto :FAIL
cix_package_tool.exe -c %PACKAGE_TOOL_PATH%\spi_flash_config_all.json -o %SAVE_BIOS_PATH%\%SAVE_BIOS_NAME%
if %errorlevel% NEQ 0 goto :FAIL
cd %WORKSPACE%

:SUCCESS
echo "Build Success"
:END
cd %WORKSPACE%
goto :EOF

:FAIL
echo "Build Failed"
goto :END
