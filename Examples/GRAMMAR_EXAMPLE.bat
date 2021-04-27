@echo off
echo ======================================
echo Testing grammar...
echo ======================================

echo.
echo.

echo TEST (Simple name and number test)
echo ======================================

echo Test source code...
echo --------------------
type TEST_GRAM
echo --------------------
PAUSE

echo Running grammar parser creator...
echo --------------------
..\Run\GRAMMAR.exe TEST_GRAM TEST_GRAM_SRC
echo --------------------
PAUSE

echo Opening generated source code...
echo --------------------
type TEST_GRAM_SRC
echo --------------------
PAUSE

del TEST_GRAM_SRC

echo ======================================



echo.
echo.
