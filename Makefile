INCLUDE=-Iinclude
HEADERS=include/alphabeta.hpp include/board.hpp include/hash.hpp include/logic.hpp include/nn.hpp include/piece.hpp include/rng.hpp include/utils.hpp
FLAGS=-Wall -flto -O3 -fopenmp -march=native
SRC=src/alphabeta.cpp src/board.cpp src/hash.cpp src/logic.cpp src/nn.cpp src/rng.cpp src/utils.cpp
OBJ=src/alphabeta.o src/board.o src/hash.o src/logic.o src/nn.o src/rng.o src/utils.o
CSHARP_SRC=src/wrap/pijersi_engine_csharp.cpp
CSHARP_OBJ=src/wrap/pijersi_engine_csharp.o
CSHARP_DLL=wrap_csharp/PijersiCore.dll

all: csharp interactive executable ugi versus

csharp: $(CSHARP_DLL)

$(CSHARP_SRC): pijersi_engine.i $(HEADERS)
	@if not exist "wrap_csharp" mkdir wrap_csharp
	@if not exist "src\wrap" mkdir src\wrap
	@swig -csharp -c++ $(INCLUDE) -namespace PijersiEngine -outdir wrap_csharp -o $(CSHARP_SRC) pijersi_engine.i

$(CSHARP_OBJ): $(CSHARP_SRC) $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) $(CSHARP_SRC) -o $(CSHARP_OBJ)

$(CSHARP_DLL): $(OBJ) $(CSHARP_OBJ)
	@if not exist "wrap_csharp" mkdir wrap_csharp
	@g++ $(FLAGS) -shared $(INCLUDE) $(OBJ) $(CSHARP_OBJ) -o $(CSHARP_DLL)

src/alphabeta.o: src/alphabeta.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/alphabeta.cpp -o src/alphabeta.o

src/board.o: src/board.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/board.cpp -o src/board.o

src/hash.o: src/hash.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/hash.cpp -o src/hash.o

src/logic.o: src/logic.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/logic.cpp -o src/logic.o

src/nn.o: src/nn.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/nn.cpp -o src/nn.o

src/mcts.o: src/mcts.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/mcts.cpp -o src/mcts.o

src/rng.o: src/rng.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/rng.cpp -o src/rng.o

src/utils.o: src/utils.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/utils.cpp -o src/utils.o


# For executables

# Interactive engine
src/interactive.o: src/interactive.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/interactive.cpp -o src/interactive.o

build/interactive.exe: $(OBJ) src/interactive.o
	@if not exist "build" mkdir build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/interactive.o -o build/interactive.exe

interactive: build/interactive.exe

# Command-line engine
src/executable.o: src/executable.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/executable.cpp -o src/executable.o

build/executable.exe: $(OBJ) src/executable.o
	@if not exist "build" mkdir build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/executable.o -o build/executable.exe

executable: build/executable.exe

# Command-line engine
src/ugi.o: src/ugi.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/ugi.cpp -o src/ugi.o

build/ugi.exe: $(OBJ) src/ugi.o
	@if not exist "build" mkdir build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/ugi.o -o build/ugi.exe

ugi: build/ugi.exe

# Match between two engines
src/versus.o: src/versus.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/versus.cpp -o src/versus.o

build/versus.exe: $(OBJ) src/versus.o
	@if not exist "build" mkdir build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/versus.o -o build/versus.exe

versus : build/versus.exe

# Debug
src/debug.o: src/debug.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/debug.cpp -o src/debug.o

build/debug.exe: $(OBJ) src/debug.o
	@if not exist "build" mkdir build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/debug.o -o build/debug.exe

debug: build/debug.exe

clean:
	@del /Q /F /S wrap_csharp\*
	@if exist "src\wrap" rmdir /Q /s "src\wrap"
	@del /Q /F /S "src\*.o"
	@if exist "build" rmdir /Q /s "build"