INCLUDE=-Iinclude
# Dependencies to nn.hpp, nn.cpp removed
HEADERS=include/alphabeta.hpp include/board.hpp include/hash.hpp include/logic.hpp include/lookup.hpp include/mcts.hpp include/openings.hpp include/options.hpp include/piece.hpp include/rng.hpp include/utils.hpp include/weights.hpp include/npy.hpp include/weight_optim.hpp
FLAGS=-Wall -flto=1 -O3 -fopenmp -std=c++20
SRC=src/alphabeta.cpp src/board.cpp src/hash.cpp src/logic.cpp src/lookup.cpp src/mcts.cpp src/options.cpp src/rng.cpp src/utils.cpp src/weight_optim.cpp
OBJ=src/alphabeta.o src/board.o src/hash.o src/logic.o src/lookup.o src/mcts.o src/options.o src/rng.o src/utils.o src/weight_optim.o

CSHARP_SRC=src/wrap/pijersi_engine_csharp.cpp
CSHARP_OBJ=src/wrap/pijersi_engine_csharp.o
CSHARP_DLL=wrap_csharp/PijersiCore.dll

PYTHON_LINK=-Lpython3.10m
PYTHON_INCLUDE=-I$(wildcard $(CONDA_PREFIX)/include/python*)
PYTHON_SRC=src/wrap/pijersi_engine_python.cpp
PYTHON_OBJ=src/wrap/pijersi_engine_python.o
PYTHON_SO=wrap_python/_PijersiCore.so

all: csharp interactive executable ugi versus

# C# wrap build

csharp: $(CSHARP_DLL)

$(CSHARP_SRC): pijersi_engine.i $(HEADERS)
	@mkdir -p wrap_csharp
	@mkdir -p src/wrap
	@swig -csharp -c++ $(INCLUDE) -namespace PijersiEngine -outdir wrap_csharp -o $(CSHARP_SRC) pijersi_engine.i

$(CSHARP_OBJ): $(CSHARP_SRC) $(HEADERS)
	@x86_64-w64-mingw32-g++-win32 $(FLAGS) -static -c $(INCLUDE) $(CSHARP_SRC) -o $(CSHARP_OBJ)

# Python wrap build

python: $(PYTHON_SO)

$(PYTHON_SRC): pijersi_engine.i $(HEADERS)
	@mkdir -p wrap_python
	@mkdir -p src/wrap
	@swig -python -c++ $(INCLUDE) -outdir wrap_python -o $(PYTHON_SRC) pijersi_engine.i

$(PYTHON_OBJ): $(PYTHON_SRC) $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) $(PYTHON_INCLUDE) $(PYTHON_LINK) $(PYTHON_SRC) -o $(PYTHON_OBJ)

$(PYTHON_SO): $(OBJ) $(PYTHON_OBJ)
	@mkdir -p wrap_python
	@g++ $(FLAGS) -shared $(INCLUDE) $(PYTHON_INCLUDE) $(PYTHON_LINK) $(OBJ) $(PYTHON_OBJ) -o $(PYTHON_SO)

$(CSHARP_DLL): $(CSHARP_OBJ)
	@mkdir -p wrap_csharp
	@x86_64-w64-mingw32-g++-win32 $(FLAGS) -static -shared $(INCLUDE) $(SRC) $(CSHARP_OBJ) -o $(CSHARP_DLL)

src/alphabeta.o: src/alphabeta.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/alphabeta.cpp -o src/alphabeta.o

src/board.o: src/board.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/board.cpp -o src/board.o

src/hash.o: src/hash.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/hash.cpp -o src/hash.o

src/logic.o: src/logic.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/logic.cpp -o src/logic.o

src/lookup.o: src/lookup.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/lookup.cpp -o src/lookup.o

src/mcts.o: src/mcts.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/mcts.cpp -o src/mcts.o

src/nn.o: src/nn.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/nn.cpp -o src/nn.o

src/options.o: src/options.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/options.cpp -o src/options.o

src/rng.o: src/rng.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/rng.cpp -o src/rng.o

src/utils.o: src/utils.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/utils.cpp -o src/utils.o

src/weight_optim.o: src/weight_optim.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/weight_optim.cpp -o src/weight_optim.o


# For executables

# Interactive engine
src/interactive.o: src/interactive.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/interactive.cpp -o src/interactive.o

build/interactive: $(OBJ) src/interactive.o
	@mkdir -p build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/interactive.o -o build/interactive

interactive: build/interactive

# Command-line engine
src/executable.o: src/executable.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/executable.cpp -o src/executable.o

build/executable: $(OBJ) src/executable.o
	@mkdir -p build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/executable.o -o build/executable

executable: build/executable

# Command-line engine
src/ugi.o: src/ugi.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/ugi.cpp -o src/ugi.o

build/ugi: $(OBJ) src/ugi.o
	@mkdir -p build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/ugi.o -o build/ugi

ugi: build/ugi

# Match between two engines
src/versus.o: src/versus.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/versus.cpp -o src/versus.o

build/versus: $(OBJ) src/versus.o
	@mkdir -p build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/versus.o -o build/versus

versus : build/versus

# Debug
src/debug.o: src/debug.cpp $(HEADERS)
	@g++ $(FLAGS) -c $(INCLUDE) src/debug.cpp -o src/debug.o

build/debug: $(OBJ) src/debug.o
	@mkdir -p build
	@g++ $(FLAGS) $(INCLUDE) $(OBJ) src/debug.o -o build/debug

debug: build/debug

clean:
	@rm -rf src/wrap
	@rm -rf wrap_csharp
	@rm -f src/*.o
	@rm -rf build