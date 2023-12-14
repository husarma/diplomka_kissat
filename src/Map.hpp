#pragma once

#include "Debug.hpp"
#include "Algorithms.hpp"

#include "instance.hpp"
#include "logger.hpp"
#include "encodings/solver_common.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

class OutputterStrategyI;

/** Class that holds information about map and agents.
*
* Delegating input generating for Picat to OutputterStrategyI.
* Calls picat for computing.
*/
class Map {
public:
    const size_t height_number_index = 7;
    const size_t width_number_index = 6;

    std::string map_file_name = "";
    std::string agents_file_name = "";

    size_t height = 0;
    size_t width = 0;

    size_t number_of_vertices = 0;
    size_t original_number_of_vertices = 0;

    std::vector<std::vector<size_t>> map;
    std::vector<std::vector<size_t>> expanded_map;
    //agent is defined by coordinates of his start pint and finish point
    std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>> agents;

    std::vector<std::vector<std::pair<size_t, size_t>>> agents_shortest_paths;

    //first in pair is time expanded graph from start of the agent and the second is from the finish
    std::pair<std::vector<std::vector<std::vector<size_t>>>, std::vector<std::vector<std::vector<size_t>>>> time_expanded_graph;

    Map();
    Map(std::string map_file_name, std::string agents_file_name);

    void set_map_file(std::string new_map_file_name);
    void set_agents_file(std::string new_agents_file_name);

    std::string load_map(std::string custom_map_file_name = "");
    std::string load_agents(int number_of_agents = -1, std::string custom_agents_file_name = "");
    std::string reload(int number_of_agents = -1);

    std::string kissat(size_t agents_time_limit, size_t time_limit_ms);

    size_t get_min_time();

    void reset_computed_map();
};
