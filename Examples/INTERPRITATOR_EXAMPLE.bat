@echo off
echo ======================================
echo Testing interpritator...
echo ======================================

echo.
echo.

echo TEST 1 (Simple number printer)
echo ======================================

echo Test source code...
echo --------------------
type TEST_INTR_PROGRAM1
echo --------------------
PAUSE

echo Running interpritator...
echo --------------------
..\Run\INTERPRITATOR.exe TEST_INTR_PROGRAM1
echo --------------------
PAUSE

echo Opening program structure graph...
echo --------------------
INTR_PROGRAM_STRUCTURE_IMG.png
echo --------------------
PAUSE

echo Opening program stack trace graph...
echo --------------------
INTR_PROGRAM_STACK_TRACE_IMG.png
echo --------------------
PAUSE

del INTR_PROGRAM_GRAMMAR_IMG.png
del INTR_PROGRAM_STRUCTURE_IMG.png
del INTR_PROGRAM_STACK_TRACE_IMG.png

echo ======================================



echo.
echo.

echo TEST 2 (Factorial)
echo ======================================

echo Test source code...
echo --------------------
type TEST_INTR_PROGRAM2
echo --------------------
PAUSE

echo Running interpritator...
echo Input number to calculate factorial...
echo --------------------
..\Run\INTERPRITATOR.exe TEST_INTR_PROGRAM2
echo --------------------
PAUSE

echo Opening program structure graph...
echo --------------------
INTR_PROGRAM_STRUCTURE_IMG.png
echo --------------------
PAUSE

echo Opening program stack trace graph...
echo --------------------
INTR_PROGRAM_STACK_TRACE_IMG.png
echo --------------------
PAUSE

del INTR_PROGRAM_GRAMMAR_IMG.png
del INTR_PROGRAM_STRUCTURE_IMG.png
del INTR_PROGRAM_STACK_TRACE_IMG.png

echo ======================================
