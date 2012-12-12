:: Build script for Microsoft Visual C++ in command prompt

@echo off

:: Check for dependencies
if not defined SYSTEMC echo Please define the SYSTEMC variable
if not defined TLM echo Please define the TLM variable
if not defined GREENSOCS echo Please define the GREENSOCS variable

:: Set compiler helpers
setlocal
set MYCOMPILE=cl /nologo /MDd /Od /W3 /c /D_CRT_SECURE_NO_DEPRECATE
set MYLINK=link /nologo
set MYMT=mt /nologo

:: Set arguments
set OUTDIR="Debug"
set INCLUDES=/I "%SYSTEMC%\..\src" /I "%TLM%\include\tlm" /I "%GREENSOCS%"
set DEFS=/D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_DLL" /D "GREENSOCKET_EXPORTS" /D "SC_INCLUDE_DYNAMIC_PROCESSES" /D "_WINDLL" /D "_UNICODE" /D "UNICODE"
set LIBPATH=/LIBPATH:"%SYSTEMC%\SystemC\Debug" /LIBPATH:"%TLM%\msvc9\tlm\Debug"
set LIBS=systemc.lib tlm.lib

if not exist %OUTDIR% mkdir %OUTDIR%


@echo on

%MYCOMPILE% %INCLUDES% %DEFS% /Gm /EHsc /RTC1 /Fo"%OUTDIR%\\" /Fd"%OUTDIR%\\vc90.pdb" /ZI /TP  /vmg ..\utils\greensocket_dll.cpp
%MYLINK% /ERRORREPORT:PROMPT %LIBPATH% /OUT:"%OUTDIR%\greensocket.dll" /INCREMENTAL /DLL /MANIFEST /MANIFESTFILE:"%OUTDIR%\greensocket.dll.intermediate.manifest" /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /DEBUG /PDB:"%OUTDIR%\greensocket.pdb" /SUBSYSTEM:WINDOWS /DYNAMICBASE /NXCOMPAT /MACHINE:X86 %LIBS% "%OUTDIR%\greensocket_dll.obj"
:: %MYMT% /out:%OUTDIR%\greensocket.dll.embed.manifest /notify_update /manifest %OUTDIR%\greensocket.dll.intermediate.manifest
