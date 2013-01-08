rem ========================================
rem ========== Compiling Vector++ ==========
rem ========================================
rem by Grigory Goltsov, 1006416

bison -y -d %1.y --define=%1.tab.h --output-file=%1.tab.c
flex -o%1.yy.c %1.l

rem ========================================
rem Renaming the files, because double
rem extensions is not a good idea in windows
rem environment 
rem ========================================
ren %1.yy.c %1yy.c
ren %1.tab.c %1tab.c

cl /Fe%1.exe /Za main.c %1yy.c %1tab.c Interpreter.c ParseTreeBuilder.c SymbolTable.c Math.c

rem ========================================
rem Cleaning up...
rem ========================================
del *.obj %1.c %1yy.c %1.yy.c %1tab.c %1.tab.h
