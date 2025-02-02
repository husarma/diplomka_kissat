#pragma once

#include "Debug.hpp"
#include <vector>
#include <queue>
#include <thread>
#include <set>

void paths_to_map(std::vector<std::vector<std::pair<size_t, size_t>>>& input_paths, std::vector<std::vector<size_t>>& map_output);

size_t give_new_numbering(std::vector<std::vector<size_t>>& map_to_renumber);

std::pair<std::string, bool> expand_map(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<size_t>>& map_to_expand, std::vector<std::vector<size_t>>& map_output);

std::vector<std::vector<size_t>> compute_time_expanded_graph(std::vector<std::vector<size_t>>& input_map, std::pair<size_t, size_t> agent);
std::set<size_t> find_used_vertices(std::vector<std::vector<size_t>>& input_map, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t time);
std::tuple<bool, size_t, std::vector<std::vector<size_t>>> cut_unreachable(std::vector<std::vector<size_t>>& map_to_cut, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t time);

std::string generate_agents_for_map(std::vector<std::vector<size_t>>& input_map, std::string map_name, std::string output_dir, size_t number_of_instances, size_t agents_in_instance);
