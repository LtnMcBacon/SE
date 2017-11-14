@echo off
copy ".\External Dependencies\lib\x64\libfbxsdk.dll" "libfbxsdk.dll"
echo Copied libfbx.sdl
.\Build\x64\FileParser.exe "Build/Asset" "ParsedAssets"
if not exist "%APPDATA%\Sluagh" mkdir %APPDATA%\Sluagh
copy /Y ".\rawLoaderEntries.txt" "%APPDATA%\Sluagh\rawLoaderEntries.txt" > nul
robocopy "ParsedAssets" "%APPDATA%\Sluagh\ParsedAssets" /S > nul
del libfbxsdk.dll > nul
del .\rawLoaderEntries.txt > nul
rd /Q /S .\ParsedAssets > nul
echo Done
pause
