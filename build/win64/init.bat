
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

echo cmake -G "Visual Studio 12 Win64" -DCMAKE_BUILD_TYPE=Debug ..\..