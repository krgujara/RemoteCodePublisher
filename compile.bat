@echo off

echo "Building project"
call "%programfiles(x86)%\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" "RemoteCodePublisher.sln" /ReBuild debug
