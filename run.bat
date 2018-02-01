@echo off

echo "Starting Remote Code Publisher"
cd  "Debug"
start "" "Client.exe"
start "" "Server.exe" C:\Users\Komal\Desktop\IISServer *.cpp *.h

pause