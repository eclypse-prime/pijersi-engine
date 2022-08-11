@echo off
if not exist "wrap" mkdir wrap
swig -csharp -c++ -Iinclude -namespace PijersiEngine -outdir wrap -o src/pijersi_engine_wrap.cxx pijersi_engine.i