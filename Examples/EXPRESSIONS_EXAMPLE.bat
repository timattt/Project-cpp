@echo off
echo ======================================
echo Testing expressions...
echo ======================================

echo.
echo.

echo Calculating simple expression [10 * 10 + 20 = ?]
echo --------------------
..\Run\EXPRESSIONS.exe --calc "10*10+20"
echo --------------------
PAUSE

echo Differentiating simple expression [d(x*x + 10 * x)/dx = ?]
echo --------------------
..\Run\EXPRESSIONS.exe --diff "(x^2)+10*x" "x"
echo --------------------
PAUSE

echo Drawing simple expression tree [x*x + 10 * x * (x + 30 + x * x)]
echo --------------------
..\Run\EXPRESSIONS.exe --draw "(x^2)+10*x*(x+30+x*x)" "TEST_EXPR"
TEST_EXPR.png
echo --------------------
PAUSE

del TEST_EXPR.png

