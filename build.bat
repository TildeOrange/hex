@echo off

del /Q bin\*

set flags=/nologo /MDd /Zi /EHa- /Febin\hex.exe /Fobin\ /Fdbin\ /Ilib\include /DGLEW_STATIC

set srcs=src\*.c

set os_libs=shell32.lib user32.lib gdi32.lib kernel32.lib opengl32.lib 
set db_libs=msvcrtd.lib glfw3.lib glew32sd.lib stb.lib
set libs=%os_libs% %db_libs%

cl %flags% %srcs% %libs% /link /libpath:lib\binaries\debug