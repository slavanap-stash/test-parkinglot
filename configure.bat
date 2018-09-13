@echo off
mkdir __build
cd __build

rem Use line below to generate Qt project instead
rem cmake -G "Visual Studio 15 Win64" ../

cmake -G "Visual Studio 15 Win64" ../ -DUSE_QT=FALSE

cd ..
pause
