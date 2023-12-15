CC = g++
CFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic -pthread
S_DIR = src
E_DIR = $(S_DIR)/encodings
B_DIR = build
PROJECT_NAME = mapf_experiments

_LIBS = libpb.a libkissat.a
LIBS = $(patsubst %,$(B_DIR)/%,$(_LIBS))

_DEPS = instance.hpp logger.hpp encodings/solver_common.hpp Algorithms.hpp BS_thread_pool.hpp Compute_Strategy.hpp Debug.hpp Map.hpp Path_finder_Strategy.hpp
DEPS = $(patsubst %,$(S_DIR)/%,$(_DEPS))

VAR = pass
MOVE = parallel pebble
FUNC = mks soc
COMP = all

_ENC_OBJ = solver_common.o $(foreach v, $(VAR), $(foreach m, $(MOVE), $(foreach f, $(FUNC), $(foreach c, $(COMP), $v_$m_$f_$c.o))))
_OBJ = main.o instance.o logger.o Algorithms.o Compute_Strategy.o Debug.o Map.o Path_finder_Strategy.o
OBJ = $(patsubst %,$(B_DIR)/%,$(_OBJ)) $(patsubst %, $(B_DIR)/%,$(_ENC_OBJ))

$(PROJECT_NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(B_DIR)/$@ $^ $(LIBS)

$(B_DIR)/%.o: $(E_DIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(B_DIR)/%.o: $(S_DIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(B_DIR)/*.o $(B_DIR)/$(PROJECT_NAME) valgrind-out.txt log.log $(B_DIR)/usecase