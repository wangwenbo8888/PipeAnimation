@echo off
rem ==== osg3DViewer : VS2010 (VC100) / Win32 / Release build ====
call "H:\VS2010\VC\bin\vcvars32.bat" || exit /b 1
cd /d "I:\Osg\osg3DViewer-master\osg3DViewer-master\build-vs2010" || exit /b 1

if "%1"=="configure" (
    cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ^
        -DQT_QMAKE_EXECUTABLE="I:/qtinstall/4.8..6/bin/qmake.exe" ^
        -DOPENSCENEGRAPH_INCLUDE_DIR="I:/Osg/osg3DViewer-master/OpenSceneGraph/include" ^
        -DOPENSCENEGRAPH_LIB_DIR="I:/Osg/osg3DViewer-master/OpenSceneGraph/lib" || exit /b 1
    exit /b 0
)

if "%1"=="build" (
    cmake --build . || exit /b 1
    exit /b 0
)

echo usage: build_vs2010.cmd [configure^|build]
