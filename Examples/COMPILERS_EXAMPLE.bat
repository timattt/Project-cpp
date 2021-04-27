@echo off
echo ======================================
echo Testing compilers...
echo ======================================

echo.
echo.

echo Test TASM source code...
echo --------------------
type TEST_TASM_PROGRAM
echo --------------------
PAUSE

echo Compiling TASM into TEXE
echo --------------------
..\Run\COMPILERS.exe --compile --texe TEST_TASM_PROGRAM TEST_TEXE
echo --------------------
PAUSE

echo Opening generated TEXE binary file...
echo --------------------
type TEST_TEXE
echo.
echo --------------------
PAUSE

echo Running TEXE...
echo --------------------
..\Run\COMPILERS.exe --run --texe TEST_TEXE
echo --------------------
PAUSE

echo Translating TEXE into EXE...
echo --------------------
..\Run\COMPILERS.exe --transToSys --texe TEST_TEXE TEST.exe
echo --------------------
PAUSE

echo Opening generated EXE binary file...
echo --------------------
type TEST.exe
echo.
echo --------------------
PAUSE

echo Running EXE...
echo --------------------
TEST.exe
echo.
echo --------------------
PAUSE

del TEST_TEXE
del TEST.exe

