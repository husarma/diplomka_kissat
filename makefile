CC = g++
CFLAGS = -std=c++20 -O3 -Wall -Wextra -pedantic -pthread
S_DIR = src
B_DIR = build
PROJECT_NAME = mapf_experiments
SOLVER = solver

_LIBS = libmapf.a
LIBS = $(patsubst %,$(B_DIR)/%,$(_LIBS))

_DEPS = MAPF.hpp Algorithms.hpp BS_thread_pool.hpp Compute_Strategy.hpp Debug.hpp Map.hpp Path_finder_Strategy.hpp
DEPS = $(patsubst %,$(S_DIR)/%,$(_DEPS))

_OBJ_MAIN = main.o Algorithms.o Compute_Strategy.o Debug.o Map.o Path_finder_Strategy.o
OBJ_MAIN = $(patsubst %,$(B_DIR)/%,$(_OBJ_MAIN))

_OBJ_SOLVER = solver.o Map.o
OBJ_SOLVER  = $(patsubst %,$(B_DIR)/%,$(_OBJ_SOLVER))

all: $(PROJECT_NAME) $(SOLVER)

$(PROJECT_NAME): $(OBJ_MAIN)
	$(CC) $(CFLAGS) -o $(B_DIR)/$@ $^ $(LIBS)

$(SOLVER): $(OBJ_SOLVER)
	$(CC) $(CFLAGS) -o $(B_DIR)/$@ $^ $(LIBS)

$(B_DIR)/%.o: $(S_DIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(B_DIR)/*.o $(B_DIR)/$(PROJECT_NAME) $(B_DIR)/$(SOLVER)