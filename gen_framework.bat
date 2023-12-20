@echo off
set DIR=%~dp0
set COCOS2DX_ROOT=%QUICK_V3_ROOT%

rd /s /q "src"

md "src\framework"
md "src\cocos"

xcopy /s /q /y "quick\framework\*.*" "src\framework"
xcopy /s /q /y "quick\cocos\*.*" "src\cocos"

%COCOS2DX_ROOT%quick\bin\compile_scripts.bat -i src -o framework_precompiled.zip

rd /s /q "src"

pause
