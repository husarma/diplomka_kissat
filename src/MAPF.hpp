#ifndef _instance_h_INCLUDED
#define _instance_h_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <queue>
#include <algorithm>

struct Vertex
{
	size_t x;
	size_t y;

	bool operator==(const Vertex &rhs) const
	{
		if (rhs.x == x && rhs.y == y)
			return true;
		return false;
    }
};

struct Agent
{
	Vertex start;
	Vertex goal;
};

class Instance
{
public:
	Instance(std::string, std::string);
	Instance(std::vector<std::vector<int> >&, std::vector<std::pair<int,int> >&, std::vector<std::pair<int,int> >&, std::string, std::string);

	void SetAgents(int);
	
	int GetMksLB(size_t);
	int GetSocLB(size_t);

	Vertex IDtoCoords(int);
	bool HasNeighbor(Vertex, int);
	bool HasNeighbor(int, int);
	int GetNeighbor(int, int);
	
	int FirstTimestep(int, int);
	int LastTimestep(int, int, int, int, int);
	int OppositeDir(int);

	void DebugPrint(std::vector<std::vector<int> >&);
	void DebugPrint(std::vector<int>&);
	void DebugPrint(std::vector<Vertex>&);

	std::vector<Agent> agents;
	std::vector<std::vector<int> > length_from_start;
	std::vector<std::vector<int> > length_from_goal;
	std::vector<int> SP_lengths;

	std::vector<std::vector<int> > map;
	size_t height;
	size_t width;
	size_t number_of_vertices;

	std::string scen_name;
	std::string map_name;

private:
	void LoadAgents(std::string, std::string);
	void LoadAgentsData(std::vector<std::pair<int,int> >&, std::vector<std::pair<int,int> >&);
	void LoadMap(std::string);
	void LoadMapData(std::vector<std::vector<int> >&);
	void BFS(std::vector<int>&, Vertex);
	
	std::vector<int> mks_LBs;
	std::vector<int> soc_LBs;
	std::vector<Vertex> coord_list;

	size_t last_number_of_agents;
};

#endif
#ifndef _logger_h_INCLUDED
#define _logger_h_INCLUDED


class Logger
{

public:
	Logger(Instance*, std::string, std::string);

	void PrintStatistics();
	void NewInstance(int);

	int solution_mks;
	int solution_soc;
	int building_time;
	int solving_time;
	int nr_vars;
	int nr_clauses;
	int solver_calls;
	
private:
	Instance* inst;
	std::string log_file;

	std::string map_name;
	std::string scen_name;
	std::string encoding;
	int agents;
	int mksLB;
	int socLB;

};

#endif
#ifndef _solver_common_h_INCLUDED
#define _solver_common_h_INCLUDED

#include <unistd.h>
#include <chrono>
#include <thread>


struct TEGAgent
{
	int first_variable;
	int first_timestep;
	int last_timestep;
};

class ISolver
{
public:
	ISolver(std::string sn, int opt) : solver_name(sn), cost_function(opt) {}; 
	virtual ~ISolver() {};
	
	int Solve(int, int = 0, bool = false);
	void SetData(Instance*, Logger*, int, bool, bool);

protected:
	Instance* inst;
	Logger* log;
	std::string solver_name;
	int cost_function; // 1 = mks, 2 = soc
	int timeout;
	bool quiet;
	bool print_plan;

	int agents;
	int vertices;
	int delta;
	int max_timestep;

	TEGAgent** at;
	TEGAgent*** pass;
	TEGAgent*** shift;
	int at_vars;

	std::vector<std::vector<int> > CNF;

	int nr_vars;
	int nr_clauses;
	int solver_calls;

	// before solving
	void PrintSolveDetails();
	void DebugPrint(std::vector<std::vector<int> >& );

	// virtual encoding to be used
	virtual int CreateFormula(std::vector<std::vector<int> >&, int) = 0;

	// creating formula
	int CreateAt(int, int);
	int CreatePass(int, int);

	void CreatePossition_Start(std::vector<std::vector<int> >&);
	void CreatePossition_Goal(std::vector<std::vector<int> >&);
	void CreatePossition_NoneAtGoal(std::vector<std::vector<int> >&);

	void CreateConf_Vertex(std::vector<std::vector<int> >&);
	void CreateConf_Swapping_At(std::vector<std::vector<int> >&);
	void CreateConf_Swapping_Pass(std::vector<std::vector<int> >&);
	void CreateConf_Pebble_At(std::vector<std::vector<int> >&);
	void CreateConf_Pebble_Pass(std::vector<std::vector<int> >&);

	void CreateMove_NoDuplicates(std::vector<std::vector<int> >&);
	void CreateMove_NextVertex_At(std::vector<std::vector<int> >&);
	void CreateMove_EnterVertex_Pass(std::vector<std::vector<int> >&);
	void CreateMove_LeaveVertex_Pass(std::vector<std::vector<int> >&);

	int CreateConst_LimitSoc(std::vector<std::vector<int> >&, int);


	// solving
	int InvokeSolver(std::vector<std::vector<int> >&, int);
	static void wait_for_terminate(int, void*, bool&);
	bool TimesUp(std::chrono::time_point<std::chrono::high_resolution_clock>, std::chrono::time_point<std::chrono::high_resolution_clock>, int);
	void CleanUp(bool);
};

/******************************************************************/
/*********************** Specific Encodings ***********************/
/******************************************************************/

class At_parallel_mks_all : public ISolver
{
public:
	using ISolver::ISolver;
	~At_parallel_mks_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class At_parallel_soc_all : public ISolver
{
public:
	using ISolver::ISolver;
	~At_parallel_soc_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class At_pebble_mks_all : public ISolver
{
public:
	using ISolver::ISolver;
	~At_pebble_mks_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class At_pebble_soc_all : public ISolver
{
public:
	using ISolver::ISolver;
	~At_pebble_soc_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class Pass_parallel_mks_all : public ISolver
{
public:
	using ISolver::ISolver;
	~Pass_parallel_mks_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class Pass_parallel_soc_all : public ISolver
{
public:
	using ISolver::ISolver;
	~Pass_parallel_soc_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class Pass_pebble_mks_all : public ISolver
{
public:
	using ISolver::ISolver;
	~Pass_pebble_mks_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

class Pass_pebble_soc_all : public ISolver
{
public:
	using ISolver::ISolver;
	~Pass_pebble_soc_all() {};

private:
	int CreateFormula(std::vector<std::vector<int> >&, int);
};

#endif
