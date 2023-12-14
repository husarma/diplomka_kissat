#pragma once

#include "Map.hpp"
#include "Path_finder_Strategy.hpp"

class PathFinderI;

/**
* Strategy design pattern.
* Interface for main solution search strategy.
*/
class ComputeStrategyI {
protected:
	std::string map_path;
	std::string agents_dir_path;
	std::string output_dir_path;

public:
	//Strategy for computing shortest paths
	std::unique_ptr<PathFinderI> path_finder;

	void set_map_file(std::string path);
	void set_agents_dir(std::string path);
	void set_output_dir(std::string path);
	void set_path_finder(std::unique_ptr<PathFinderI> p);

	virtual ~ComputeStrategyI() {}
	virtual std::string run_tests(size_t time_limit=300000) = 0;
};

//////////////////////////////////////// Kissat usage ////////////////////////////////////////

/** Concrete implementation of search strategy for finding an solution.*/
class Baseline : public ComputeStrategyI {
	std::string run_tests(size_t time_limit = 300000) override;
};

/** Concrete implementation of search strategy for finding an solution.*/
class MakespanAdd : public ComputeStrategyI {
	std::string run_tests(size_t time_limit = 300000) override;
};

/** Concrete implementation of search strategy for finding an solution.*/
class PruningCut : public ComputeStrategyI {
	std::string run_tests(size_t time_limit = 300000) override;
};

/** Concrete implementation of search strategy for finding an solution.*/
class Combined : public ComputeStrategyI {
	std::string run_tests(size_t time_limit = 300000) override;
};
