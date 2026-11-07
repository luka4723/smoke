@echo off
 
set FPS=%1
if "%FPS%"=="" set FPS=30
 
if not exist frames\frame_0000.ppm (
    echo Nema frames\frame_0000.ppm - prvo pokreni simulaciju.
    exit /b 1
)
 
ffmpeg -y -framerate %FPS% -i frames\frame_%%04d.ppm -c:v libx264 -preset veryfast -crf 18 -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" smoke.mp4
ffmpeg -y -framerate %FPS% -i frames\frame_%%04d.ppm -vf "scale=384:-1:flags=neighbor,split[a][b];[a]palettegen[p];[b][p]paletteuse" smoke.gif
 