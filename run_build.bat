@echo off
rem -------------------------------------------------
rem Clean build + unit-test for Course-Helper
rem -------------------------------------------------

rem 1. Delete old build directory
if exist build (
    echo Removing old build directory ...
    rmdir /s /q build
)

rem 2. Recreate build directory and cd into it
mkdir build
cd build

rem 3. CMake configuration
rem 逻辑：优先使用命令行传入的参数，如果没有，再找环境变量，最后用本地硬编码路径兜底
if "%QT_DIR%"=="" set QT_DIR=C:\Qt\6.10.0\msvc2022_64

echo Using Qt from: %QT_DIR%

cmake -S ".." -B . -G "NMake Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%"

rem 4. Build Release target
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed
    exit /b 1
)

rem 5. Run unit tests (ctest will discover DataStoreTest)
ctest -C Release --output-on-failure
if errorlevel 1 (
    echo [ERROR] Some tests failed
    exit /b 1
)

echo -------------------------------------------------
echo Build and test succeeded.
echo -------------------------------------------------