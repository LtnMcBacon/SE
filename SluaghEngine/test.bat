start /B /WAIT "" Game.exe
for /f "tokens=1-5 delims=:" %%d in ("%time%") do rename "StreamTimings.csv" "StreamTimings%%d-%%e-%%f.csv"