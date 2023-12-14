#include "Algorithms.hpp"

#include <algorithm>

/** Writes paths to map.
*
* @param input_paths vector of pairs including agents paths.
* @param map_output map for result writing.
*/
void paths_to_map(std::vector<std::vector<std::pair<size_t, size_t>>>& input_paths, std::vector<std::vector<size_t>>& map_output) {

	for (size_t i = 0; i < input_paths.size(); i++) {
		for (size_t j = 0; j < input_paths[i].size(); j++) {
			map_output[input_paths[i][j].first][input_paths[i][j].second] = 1;
		}
	}
}

/** Expand free vertices in map by souranding the existing ones.
*
* Can write the result back to the same map, but it is slower,
* because it need to copy the map for computing
*
* @param reference_map original input map.
* @param map_to_expand map to be expanded.
* @param map_output map for result writing, can be the same as expanded map.
* @return error message, "OK" if everything ended right and true if expanded in pair.
*/
std::pair<std::string, bool> expand_map(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<size_t>>& map_to_expand, std::vector<std::vector<size_t>>& map_output) {

	if (reference_map.size() != map_output.size() || reference_map[0].size() != map_output[0].size() ||
		map_to_expand.size() != map_output.size() || map_to_expand[0].size() != map_output[0].size() ||
		reference_map.size() != map_to_expand.size() || reference_map[0].size() != map_to_expand[0].size()) {
		return std::make_pair("ERROR: different lenghts of maps in surroundings\n", false);
	}

	bool same = false;
	std::vector<std::vector<size_t>> copy;
	if (&map_to_expand == &map_output) {
		same = true;
		copy = map_to_expand;
	}

	bool expanded = false;

	bool surround = false;
	for (size_t i = 0; i < reference_map.size(); i++) {
		for (size_t j = 0; j < reference_map[0].size(); j++) {

			if (same) {
				if (copy[i][j] != 0) {
					surround = true;
				}
			}
			else if (map_to_expand[i][j] != 0) {
				surround = true;
			}

			if (surround) {
				if (reference_map[i - 1][j] != 0) {
					if (!map_output[i - 1][j]) {
						expanded = true;
					}
					map_output[i - 1][j] = 1;
				}
				if (reference_map[i][j + 1] != 0) {
					if (!map_output[i][j + 1]) {
						expanded = true;
					}
					map_output[i][j + 1] = 1;
				}
				if (reference_map[i + 1][j] != 0) {
					if (!map_output[i + 1][j]) {
						expanded = true;
					}
					map_output[i + 1][j] = 1;
				}
				if (reference_map[i][j - 1] != 0) {
					if (!map_output[i][j - 1]) {
						expanded = true;
					}
					map_output[i][j - 1] = 1;
				}

				surround = false;
			}
		}
	}

	if (expanded) {
		return std::make_pair("OK", true);
	}
	else {
		return std::make_pair("OK", false);
	}


}

/** Gives new appropriete number to each vertex.
*
* @param map_to_renumber map to be renumbered.
*/
size_t give_new_numbering(std::vector<std::vector<size_t>>& map_to_renumber) {

	size_t vertex_number = 1;
	for (size_t i = 0; i < map_to_renumber.size(); i++) {
		for (size_t j = 0; j < map_to_renumber[0].size(); j++) {
			if (map_to_renumber[i][j] != 0) {
				map_to_renumber[i][j] = vertex_number;
				vertex_number++;
			}
		}
	}

	return vertex_number - 1;
}

std::vector<std::vector<size_t>> time_expanded_graph(std::vector<std::vector<size_t>>& input_map, std::pair<size_t, size_t> agent) {

	std::vector<std::vector<size_t>> temp_map(input_map.size(), std::vector<size_t>(input_map[0].size(), 0));

	std::queue<std::pair<size_t, size_t>> vertex_queue;

	size_t t = 0;
	vertex_queue.push(agent);
	temp_map[agent.first][agent.second] = 1;

	while (!vertex_queue.empty()) {

		//push lap delimiter
		vertex_queue.push(std::make_pair(0, 0));

		t++;
		while (vertex_queue.front().first != 0) {
			std::pair<size_t, size_t> a = vertex_queue.front();
			vertex_queue.pop();

			if (input_map[a.first - 1][a.second] != 0 && temp_map[a.first - 1][a.second] == 0) {
				temp_map[a.first - 1][a.second] = t;
				vertex_queue.push(std::make_pair(a.first - 1, a.second));
			}
			if (input_map[a.first][a.second + 1] != 0 && temp_map[a.first][a.second + 1] == 0) {
				temp_map[a.first][a.second + 1] = t;
				vertex_queue.push(std::make_pair(a.first, a.second + 1));
			}
			if (input_map[a.first + 1][a.second] != 0 && temp_map[a.first + 1][a.second] == 0) {
				temp_map[a.first + 1][a.second] = t;
				vertex_queue.push(std::make_pair(a.first + 1, a.second));
			}
			if (input_map[a.first][a.second - 1] != 0 && temp_map[a.first][a.second - 1] == 0) {
				temp_map[a.first][a.second - 1] = t;
				vertex_queue.push(std::make_pair(a.first, a.second - 1));
			}
		}

		//pop lap delimiter
		vertex_queue.pop();
	}

	temp_map[agent.first][agent.second] = 0;
	return temp_map;
}

std::set<size_t> find_used_vertices(std::vector<std::vector<size_t>>& input_map, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t time) {

	std::set<size_t> used_vertices;

	for (size_t a = 0; a < agents.size(); a++) {

		auto start_to_finish = time_expanded_graph(input_map, agents[a].first);
		auto finish_to_start = time_expanded_graph(input_map, agents[a].second);

		for (size_t i = 0; i < start_to_finish.size(); i++) {
			for (size_t j = 0; j < start_to_finish[0].size(); j++) {
				if (input_map[i][j] != 0) {
					if (start_to_finish[i][j] + finish_to_start[i][j] <= time) {
						used_vertices.insert(input_map[i][j]);
					}
				}
			}
		}
	}

	return used_vertices;
}

std::tuple<bool, size_t, std::vector<std::vector<size_t>>> cut_unreachable(std::vector<std::vector<size_t>>& map_to_cut, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t time) {

	std::vector<std::vector<size_t>> map_output = map_to_cut;

	bool cutted = false;
	give_new_numbering(map_output);
	std::set<size_t> used_vertices = find_used_vertices(map_output, agents, time);
	for (size_t i = 0; i < map_output.size(); i++) {
		for (size_t j = 0; j < map_output[0].size(); j++) {
			if (used_vertices.find(map_output[i][j]) == used_vertices.end()) {
				map_output[i][j] = 0;
			}
			if (map_output[i][j] == 0 && map_to_cut[i][j] != 0) {
				cutted = true;
			}
		}
	}
	if (cutted) {
		size_t number_of_vertices = give_new_numbering(map_output);
		return std::make_tuple(true, number_of_vertices, map_output);
	}
	else {
		size_t number_of_vertices = give_new_numbering(map_output);
		return std::make_tuple(false, number_of_vertices, map_output);
	}
}

/** Generate files containing instances for given map.
*
* @param input_map map to generate for.
* @param map_name name of map.
* @param output_dir path to directory for outputing generated instances.
* @param number_of_instances number of generated instances.
* @param agents_in_instance number of agents in one instance.
* @return error message, "OK" if everything ended right.
*/
std::string generate_agents_for_map(std::vector<std::vector<size_t>>& input_map, std::string map_name, std::string output_dir, size_t number_of_instances, size_t agents_in_instance) {

	std::string::size_type const map_dot(map_name.find_last_of('.'));

	std::string clear_map_name = map_name.substr(0, map_dot);

	std::vector<std::pair<size_t, size_t>> agent_start;
	std::vector<std::pair<size_t, size_t>> agent_goal;

	for (size_t i = 0; i < number_of_instances; i++) {

		agent_start.clear();
		agent_goal.clear();

		std::ofstream ofile;
		ofile.open(output_dir + "\\" + clear_map_name + "-even-" + std::to_string(i + 1) + ".scen");

		if (ofile.is_open()) {

			ofile << "version 1" << std::endl;

			for (size_t j = 0; j < agents_in_instance; j++) {
				size_t s_x;
				size_t s_y;
				size_t g_x;
				size_t g_y;
				bool finded = false;

				while (!finded) {
					s_x = (std::rand() % (input_map[0].size() - 2)) + 1;
					s_y = (std::rand() % (input_map[0].size() - 2)) + 1;


					if ((agent_start.end() == std::find(agent_start.begin(), agent_start.end(), std::make_pair(s_x, s_y))) && (input_map[s_x][s_y] != 0)) { //if not allready used and is free in map

						agent_start.push_back(std::make_pair(s_x, s_y));

						while (!finded) {
							g_x = (std::rand() % (input_map[0].size() - 2)) + 1;
							g_y = (std::rand() % (input_map[0].size() - 2)) + 1;


							if ((agent_goal.end() == std::find(agent_goal.begin(), agent_goal.end(), std::make_pair(g_x, g_y))) && (input_map[g_x][g_y] != 0)) { //if not allready used and is free in map

								agent_goal.push_back(std::make_pair(g_x, g_y));
								finded = true;
							}
						}
					}
				}

				ofile <<
					"0\t"
					+ map_name +
					"\t"
					+ std::to_string(input_map[0].size() - 2) +
					"\t"
					+ std::to_string(input_map[0].size() - 2) +
					"\t"
					+ std::to_string(s_y - 1) +
					"\t"
					+ std::to_string(s_x - 1) +
					"\t"
					+ std::to_string(g_y - 1) +
					"\t"
					+ std::to_string(g_x - 1) +
					"\t0" << std::endl;
			}

		}
		else {
			return "ERROR: cannot open file for writing agents instance : " + output_dir + "\\" + clear_map_name + "-even-" + std::to_string(i) + "\n";
		}

		ofile.close();
	}

	return "OK";
}