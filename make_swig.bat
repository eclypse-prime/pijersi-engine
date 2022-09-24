@echo off
if not exist "wrap_csharp" mkdir wrap_csharp
swig -csharp -c++ -Iinclude -namespace PijersiEngine -outdir wrap_csharp -o src/pijersi_engine_wrap.cxx pijersi_engine.i