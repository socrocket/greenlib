Building the GreenSocket library as a DLL
-----------------------------------------

1) First you will need a modified version of the TLM library that is compatible
with the DLL build process. Please contact GreenSocs for more information.

2) Set the following environment variables acording to your installation:

   * SYSTEMC to the path containing the installed SystemC compiled as DLL
   example:   set SYSTEMC=C:\tools\systemc-2.2.0\msvc71
   * TLM to the path containing the modified TLM compiled as DLL
   example:   set TLM=C:\tools\tlm-2.0
   * GREENSOCS to the path where the GreenSocs tarball were extracted
   example:   set GREENSOCS=c:\tools\greensocs

3) Do the MSVC command-line initialization if not already done, normally a call
to:
   "C:\Program Files\Microsoft Visual Studio 9.0\VC\bin\vcvars32.bat"

4) Finally run:
   build_dll.bat


The resulting greensocket.dll will be in the Debug directory by default.
