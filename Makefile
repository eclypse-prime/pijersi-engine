INCLUDE=-Iinclude -Itensorflow/include
LIB=-Ltensorflow/lib
HEADERS=include/board.h include/piece.h
SRC=src/board.cpp
OBJ=src/board.o
CSHARP_SRC=src/wrap/pijersi_engine_csharp.cpp
CSHARP_OBJ=src/wrap/pijersi_engine_csharp.o
CSHARP_DLL=wrap_csharp/PijersiCore.dll

csharp: $(CSHARP_DLL)

wrap_csharp/*.cs src/wrap/pijersi_engine_csharp.cpp: pijersi_engine.i $(HEADERS)
	@if not exist "wrap_csharp" mkdir wrap_csharp
	@if not exist "src/wrap" mkdir src/wrap
	@swig -csharp -c++ $(INCLUDE) -namespace PijersiEngine -outdir wrap_csharp -o $(CSHARP_SRC) pijersi_engine.i

src/board.o: src/board.cpp $(HEADERS)
	@g++ -flto -c -O3 -fopenmp $(INCLUDE) src/board.cpp -o src/board.o

src/wrap/pijersi_engine_csharp.o: $(CSHARP_SRC) $(HEADERS)
	@g++ -flto -c -O3 -fopenmp $(INCLUDE) $(CSHARP_SRC) -o $(CSHARP_OBJ)

$(CSHARP_DLL): $(OBJ) $(CSHARP_OBJ)
	@if not exist "wrap_csharp" mkdir wrap_csharp
	@g++ -flto -O3 -fopenmp -shared $(INCLUDE) $(OBJ) $(CSHARP_OBJ) -o $(CSHARP_DLL)

debug: $(SRC) src/debug.cpp
	@g++ -ggdb -O3 -flto -fopenmp $(INCLUDE) $(SRC) src/debug.cpp -o build/debug.exe

clean:
	@del /Q /F /S wrap_csharp\*
	@del /Q /F /S "src\wrap\*.cpp"
	@del /Q /F /S "src\*.o"