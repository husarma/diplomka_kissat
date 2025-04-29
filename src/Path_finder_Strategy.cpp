#include "Path_finder_Strategy.hpp"

#include <algorithm>
#include <random>

/** Computes the shortest paths in graph for agents.
*
* For its bias uses BFS algorithm with basic reconstruction of finded path.
*
* @param reference_map original input map.
* @param output_paths vector for writing the result for agents.
* @param agents vector of pairs containig agents start and finish coordinates in pair.
* @return error message, "OK" if everything ended right.
*/
std::string Biaset::compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t makespan) {
	if (output_paths.size() != agents.size()) {
		return "ERROR: different lenghts of paths and agents\n";
	}

	output_paths.clear();
	output_paths.resize(agents.size());

	std::queue<std::pair<size_t, size_t>> vertex_queue;

	//Compute all agents
	for (size_t i = 0; i < agents.size(); i++) {

		std::vector<std::vector<std::pair<size_t, size_t>>> temp_map(reference_map.size(), std::vector<std::pair<size_t, size_t>>(reference_map[0].size(), std::make_pair(0, 0)));

		vertex_queue = std::queue<std::pair<size_t, size_t>>();

		bool found = false;
		vertex_queue.push(agents[i].second);

		while (!vertex_queue.empty() && found == false) {
			std::pair<size_t, size_t> a = vertex_queue.front();
			vertex_queue.pop();

			//going from finish to start for better reproduction of the path
			if (a.first == agents[i].first.first && a.second == agents[i].first.second) {
				found = true;
			}

			if (reference_map[a.first - 1][a.second] != 0 && temp_map[a.first - 1][a.second].first == 0) {
				temp_map[a.first - 1][a.second].first = a.first;
				temp_map[a.first - 1][a.second].second = a.second;
				vertex_queue.push(std::make_pair(a.first - 1, a.second));
			}
			if (reference_map[a.first][a.second + 1] != 0 && temp_map[a.first][a.second + 1].first == 0) {
				temp_map[a.first][a.second + 1].first = a.first;
				temp_map[a.first][a.second + 1].second = a.second;
				vertex_queue.push(std::make_pair(a.first, a.second + 1));
			}
			if (reference_map[a.first + 1][a.second] != 0 && temp_map[a.first + 1][a.second].first == 0) {
				temp_map[a.first + 1][a.second].first = a.first;
				temp_map[a.first + 1][a.second].second = a.second;
				vertex_queue.push(std::make_pair(a.first + 1, a.second));
			}
			if (reference_map[a.first][a.second - 1] != 0 && temp_map[a.first][a.second - 1].first == 0) {
				temp_map[a.first][a.second - 1].first = a.first;
				temp_map[a.first][a.second - 1].second = a.second;
				vertex_queue.push(std::make_pair(a.first, a.second - 1));
			}
		}

		//reproducting the path from start to finish
		if (found) {
			std::pair<size_t, size_t> current = agents[i].first;
			output_paths[i].push_back(std::make_pair(agents[i].first.first, agents[i].first.second));
			while (current.first != agents[i].second.first || current.second != agents[i].second.second) {
				current = temp_map[current.first][current.second];
				output_paths[i].push_back(std::make_pair(current.first, current.second));
			}
		}

	}

	//check if each agent found his finis
	for (size_t a = 0; a < agents.size(); a++) {
		if (output_paths[a].size() == 0) {
			return "ERROR: some agents can't get to finish";
		}
	}

	//make all path the same length
	for (size_t a = 0; a < agents.size(); a++) {
		while(output_paths[a].size() < makespan) {
			output_paths[a].push_back(output_paths[a].back());
		}
	}

	return "OK";
}

std::string Biaset::get_name() {
	return "Biaset";
}

/** Computes the shortest paths in graph for agents.
*
* Uses BFS algorithm with random reconstruction of path (looks to posible vertices wrom witch could came from and chooses random one).
*
* @param reference_map original input map.
* @param output_paths vector for writing the result for agents.
* @param agents vector of pairs containig agents start and finish coordinates in pair.
* @return error message, "OK" if everything ended right.
*/
std::string TrullyRandom::compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t makespan) {
	if (output_paths.size() != agents.size()) {
		return "ERROR: different lenghts of paths and agents\n";
	}

	output_paths.clear();
	output_paths.resize(agents.size());

	std::queue<std::pair<size_t, size_t>> vertex_queue;

	//Compute all agents
	for (size_t i = 0; i < agents.size(); i++) {

		//vector of vector 2D map of pairs of time and vector of all possible points from which was able to came to that point in time t
		std::vector<std::vector<std::pair<size_t, std::vector<std::pair<size_t, size_t>>>>> temp_map(reference_map.size(), std::vector<std::pair<size_t, std::vector<std::pair<size_t, size_t>>>>(reference_map[0].size(), std::make_pair(0, std::vector<std::pair<size_t, size_t>>())));

		vertex_queue = std::queue<std::pair<size_t, size_t>>();

		size_t t = 0;

		bool found = false;
		vertex_queue.push(agents[i].second);

		temp_map[agents[i].second.first][agents[i].second.second].first = 1;

		while (!vertex_queue.empty() && found == false) {

			//push lap delimiter
			vertex_queue.push(std::make_pair(0, 0));
			t++;

			while (vertex_queue.front().first != 0) {
				std::pair<size_t, size_t> a = vertex_queue.front();
				vertex_queue.pop();

				//going from finish to start for better reproduction of the path
				if (a.first == agents[i].first.first && a.second == agents[i].first.second) {
					found = true;
				}

				if (reference_map[a.first - 1][a.second] != 0 && (temp_map[a.first - 1][a.second].first == 0 || temp_map[a.first - 1][a.second].first == t - 1)) {
					if (temp_map[a.first - 1][a.second].first == 0) {
						temp_map[a.first - 1][a.second].first = t + 1;
						vertex_queue.push(std::make_pair(a.first - 1, a.second));
					}
					
					if (temp_map[a.first - 1][a.second].first == t - 1)
						temp_map[a.first][a.second].second.push_back(std::make_pair(a.first - 1, a.second));
				}
				if (reference_map[a.first][a.second + 1] != 0 && (temp_map[a.first][a.second + 1].first == 0 || temp_map[a.first][a.second + 1].first == t - 1)) {
					if (temp_map[a.first][a.second + 1].first == 0) {
						temp_map[a.first][a.second + 1].first = t + 1;
						vertex_queue.push(std::make_pair(a.first, a.second + 1));
					}
					
					if (temp_map[a.first][a.second + 1].first == t - 1)
						temp_map[a.first][a.second].second.push_back(std::make_pair(a.first, a.second + 1));
				}
				if (reference_map[a.first + 1][a.second] != 0 && (temp_map[a.first + 1][a.second].first == 0 || temp_map[a.first + 1][a.second].first == t - 1)) {
					if (temp_map[a.first + 1][a.second].first == 0) {
						temp_map[a.first + 1][a.second].first = t + 1;
						vertex_queue.push(std::make_pair(a.first + 1, a.second));
					}
					
					if (temp_map[a.first + 1][a.second].first == t - 1)
						temp_map[a.first][a.second].second.push_back(std::make_pair(a.first + 1, a.second));
				}
				if (reference_map[a.first][a.second - 1] != 0 && (temp_map[a.first][a.second - 1].first == 0 || temp_map[a.first][a.second - 1].first == t - 1)) {
					if (temp_map[a.first][a.second - 1].first == 0) {
						temp_map[a.first][a.second - 1].first = t + 1;
						vertex_queue.push(std::make_pair(a.first, a.second - 1));
					}
					
					if (temp_map[a.first][a.second - 1].first == t - 1)
						temp_map[a.first][a.second].second.push_back(std::make_pair(a.first, a.second - 1));
				}
			}

			//pop lap delimiter
			vertex_queue.pop();
		}

		//seeding rnadom generator for same results
		std::mt19937 gen;
		gen.seed(31102001);

		//reproducting the path from start to finish
		if (found) {
			std::pair<size_t, size_t> current = agents[i].first;
			output_paths[i].push_back(std::make_pair(agents[i].first.first, agents[i].first.second));
			while (current.first != agents[i].second.first || current.second != agents[i].second.second) {
				std::sample(
					temp_map[current.first][current.second].second.begin(),
					temp_map[current.first][current.second].second.end(),
					std::back_inserter(output_paths[i]),
					1,
					gen
				);
				current = output_paths[i].back();
			}
		}
	}

	//check if each agent found his finis
	for (size_t a = 0; a < agents.size(); a++) {
		if (output_paths[a].size() == 0) {
			return "ERROR: some agents can't get to finish";
		}
	}

	//make all path the same length
	for (size_t a = 0; a < agents.size(); a++) {
		while(output_paths[a].size() < makespan) {
			output_paths[a].push_back(output_paths[a].back());
		}
	}

	return "OK";
}

std::string TrullyRandom::get_name() {
	return "Random";
}

/** WCR heuristic comparison class. */
class CompareWithoutCrossing {
public:
	std::pair<size_t, size_t> finish;
	std::vector<std::vector<size_t>>* dep_crossing_to_get_there = nullptr;
	std::vector<std::vector<std::pair<size_t, std::pair<size_t, size_t>>>>* dep_temp_map = nullptr;

	CompareWithoutCrossing() {
		dep_crossing_to_get_there = nullptr;
		dep_temp_map = nullptr;
	}

	CompareWithoutCrossing(std::pair<size_t, size_t> finish, std::vector<std::vector<size_t>>& crossing_to_get_there, std::vector<std::vector<std::pair<size_t, std::pair<size_t, size_t>>>>& temp_map) :
		finish(finish),
		dep_crossing_to_get_there(&crossing_to_get_there),
		dep_temp_map(&temp_map)
	{}

	bool operator()(std::pair<size_t, size_t> above, std::pair<size_t, size_t> below) const {
		const std::vector<std::vector<std::pair<size_t, std::pair<size_t, size_t>>>>& temp_map_reference = *dep_temp_map;
		const std::vector<std::vector<size_t>>& crossing_to_get_there_reference = *dep_crossing_to_get_there;

		size_t below_g = temp_map_reference[below.first][below.second].first;
		size_t below_h = abs((int)below.first - (int)finish.first) + abs((int)below.second - (int)finish.second);
		size_t below_f = below_g + below_h;
		size_t below_used_times = crossing_to_get_there_reference[below.first][below.second];

		size_t above_g = temp_map_reference[above.first][above.second].first;
		size_t above_h = abs((int)above.first - (int)finish.first) + abs((int)above.second - (int)finish.second);
		size_t above_f = above_g + above_h;
		size_t above_used_times = crossing_to_get_there_reference[above.first][above.second];

		if (below_f < above_f) {
			return true;
		}
		if (below_f > above_f) {
			return false;
		}
		//f() of both are equal
		if (below_used_times < above_used_times) {
			return true;
		}
		if (below_used_times > above_used_times) {
			return false;
		}
		//they are equal in everything
		return false;
	}
};

/** Computes the shortest paths in graph for agents.
*
* Uses A* algorithm with WCR heuristic.
*
* @param reference_map original input map.
* @param output_paths vector for writing the result for agents.
* @param agents vector of pairs containig agents start and finish coordinates in pair.
* @return error message, "OK" if everything ended right.
*/
std::string WithoutCrossing::compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t makespan) {
	if (output_paths.size() != agents.size()) {
		return "ERROR: different lenghts of paths and agents\n";
	}

	output_paths.clear();
	output_paths.resize(agents.size());

	std::vector<std::vector<size_t>> used_number_of_vertex = std::vector<std::vector<size_t>>(reference_map.size(), std::vector<size_t>(reference_map[0].size(), 0));
	
	std::vector<std::pair<size_t, size_t>> priority_queue = std::vector<std::pair<size_t, size_t>>();

	//Compute all agents
	for (size_t i = 0; i < agents.size(); i++) {

		//vector of vector 2D map of pairs of g() whitch is time and point from which was came to that point with smallest g()
		std::vector<std::vector<std::pair<size_t, std::pair<size_t, size_t>>>> temp_map(reference_map.size(), std::vector<std::pair<size_t, std::pair<size_t, size_t>>>(reference_map[0].size(), std::make_pair(SIZE_MAX, std::pair<size_t, size_t>())));
		std::vector<std::vector<size_t>> crossing_to_get_there = std::vector<std::vector<size_t>>(reference_map.size(), std::vector<size_t>(reference_map[0].size(), 0));

		CompareWithoutCrossing comparator = CompareWithoutCrossing(agents[i].first, crossing_to_get_there, temp_map);
		std::vector<std::pair<size_t, size_t>> priority_queue = std::vector<std::pair<size_t, size_t>>();

		bool found = false;
		priority_queue.push_back(agents[i].second);
		temp_map[agents[i].second.first][agents[i].second.second].first = 1;

		while (!priority_queue.empty() && found == false) {
			std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			std::pair<size_t, size_t> a = priority_queue.back();
			priority_queue.pop_back();

			size_t current_g = temp_map[a.first][a.second].first;
			size_t current_cross = crossing_to_get_there[a.first][a.second];

			//going from finish to start for better reproduction of the path
			if (a.first == agents[i].first.first && a.second == agents[i].first.second) {
				found = true;
			}

			if (reference_map[a.first - 1][a.second] != 0 && temp_map[a.first - 1][a.second].first > current_g + 1) {
				temp_map[a.first - 1][a.second].second.first = a.first;
				temp_map[a.first - 1][a.second].second.second = a.second;
				temp_map[a.first - 1][a.second].first = current_g + 1;
				if (used_number_of_vertex[a.first - 1][a.second] != 0) {
					crossing_to_get_there[a.first - 1][a.second] = current_cross + 1;
				}
				else {
					crossing_to_get_there[a.first - 1][a.second] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first - 1, a.second));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
			if (reference_map[a.first][a.second + 1] != 0 && temp_map[a.first][a.second + 1].first > current_g + 1) {
				temp_map[a.first][a.second + 1].second.first = a.first;
				temp_map[a.first][a.second + 1].second.second = a.second;
				temp_map[a.first][a.second + 1].first = current_g + 1;
				if (used_number_of_vertex[a.first][a.second + 1] != 0) {
					crossing_to_get_there[a.first][a.second + 1] = current_cross + 1;
				}
				else {
					crossing_to_get_there[a.first][a.second + 1] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first, a.second + 1));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
			if (reference_map[a.first + 1][a.second] != 0 && temp_map[a.first + 1][a.second].first > current_g + 1) {
				temp_map[a.first + 1][a.second].second.first = a.first;
				temp_map[a.first + 1][a.second].second.second = a.second;
				temp_map[a.first + 1][a.second].first = current_g + 1;
				if (used_number_of_vertex[a.first + 1][a.second] != 0) {
					crossing_to_get_there[a.first + 1][a.second] = current_cross + 1;
				}
				else {
					crossing_to_get_there[a.first + 1][a.second] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first + 1, a.second));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
			if (reference_map[a.first][a.second - 1] != 0 && temp_map[a.first][a.second - 1].first > current_g + 1) {
				temp_map[a.first][a.second - 1].second.first = a.first;
				temp_map[a.first][a.second - 1].second.second = a.second;
				temp_map[a.first][a.second - 1].first = current_g + 1;
				if (used_number_of_vertex[a.first][a.second - 1] != 0) {
					crossing_to_get_there[a.first][a.second - 1] = current_cross + 1;
				}
				else {
					crossing_to_get_there[a.first][a.second - 1] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first, a.second - 1));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
		}

		//reproducting the path from start to finish
		if (found) {
			std::pair<size_t, size_t> current = agents[i].first;
			output_paths[i].push_back(std::make_pair(agents[i].first.first, agents[i].first.second));
			used_number_of_vertex[current.first][current.second]++;
			while (current.first != agents[i].second.first || current.second != agents[i].second.second) {
				current = temp_map[current.first][current.second].second;
				output_paths[i].push_back(std::make_pair(current.first, current.second));
				used_number_of_vertex[current.first][current.second]++;
			}
		}

	}

	//check if each agent found his finis
	for (size_t a = 0; a < agents.size(); a++) {
		if (output_paths[a].size() == 0) {
			return "ERROR: some agents can't get to finish";
		}
	}

	//make all path the same length
	for (size_t a = 0; a < agents.size(); a++) {
		while(output_paths[a].size() < makespan) {
			output_paths[a].push_back(output_paths[a].back());
		}
	}

	return "OK";
}

std::string WithoutCrossing::get_name() {
	return "WithoutCrossing";
}

/** Computes the shortest paths in graph for agents.
*
* Uses A* algorithm with XCR heuristic (Cleverly uses WCR as XCR).
*
* @param reference_map original input map.
* @param output_paths vector for writing the result for agents.
* @param agents vector of pairs containig agents start and finish coordinates in pair.
* @return error message, "OK" if everything ended right.
*/
std::string WithoutCrossingAtSameTimes::compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t makespan) {
	if (output_paths.size() != agents.size()) {
		return "ERROR: different lenghts of paths and agents\n";
	}

	output_paths.clear();
	output_paths.resize(agents.size());

	std::vector<std::vector<std::vector<size_t>>> used_number_of_vertex(reference_map.size(), std::vector<std::vector<size_t>>(reference_map[0].size()));

	std::vector<std::pair<size_t, size_t>> priority_queue = std::vector<std::pair<size_t, size_t>>();

	//Compute all agents
	for (size_t i = 0; i < agents.size(); i++) {

		//vector of vector 2D map of pairs of g() whitch is time and point from which was came to that point with smallest g()
		std::vector<std::vector<std::pair<size_t, std::pair<size_t, size_t>>>> temp_map(reference_map.size(), std::vector<std::pair<size_t, std::pair<size_t, size_t>>>(reference_map[0].size(), std::make_pair(SIZE_MAX, std::pair<size_t, size_t>())));
		std::vector<std::vector<size_t>> crossing_to_get_there = std::vector<std::vector<size_t>>(reference_map.size(), std::vector<size_t>(reference_map[0].size(), 0));

		CompareWithoutCrossing comparator = CompareWithoutCrossing(agents[i].first, crossing_to_get_there, temp_map);
		std::vector<std::pair<size_t, size_t>> priority_queue = std::vector<std::pair<size_t, size_t>>();

		bool found = false;
		priority_queue.push_back(agents[i].second);
		temp_map[agents[i].second.first][agents[i].second.second].first = 1;

		while (!priority_queue.empty() && found == false) {
			std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			std::pair<size_t, size_t> a = priority_queue.back();
			priority_queue.pop_back();

			size_t current_g = temp_map[a.first][a.second].first;
			size_t current_cross = crossing_to_get_there[a.first][a.second];

			//going from finish to start for better reproduction of the path
			if (a.first == agents[i].first.first && a.second == agents[i].first.second) {
				found = true;
			}

			if (reference_map[a.first - 1][a.second] != 0 && temp_map[a.first - 1][a.second].first > current_g + 1) {
				temp_map[a.first - 1][a.second].second.first = a.first;
				temp_map[a.first - 1][a.second].second.second = a.second;
				temp_map[a.first - 1][a.second].first = current_g + 1;
				std::vector<size_t> used_in_times_vec = used_number_of_vertex[a.first - 1][a.second];
				size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), current_g + 1);
				if (used_times != 0) {
					crossing_to_get_there[a.first - 1][a.second] = current_cross + used_times;
				}
				else {
					crossing_to_get_there[a.first - 1][a.second] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first - 1, a.second));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
			if (reference_map[a.first][a.second + 1] != 0 && temp_map[a.first][a.second + 1].first > current_g + 1) {
				temp_map[a.first][a.second + 1].second.first = a.first;
				temp_map[a.first][a.second + 1].second.second = a.second;
				temp_map[a.first][a.second + 1].first = current_g + 1;
				std::vector<size_t> used_in_times_vec = used_number_of_vertex[a.first][a.second + 1];
				size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), current_g + 1);
				if (used_times != 0) {
					crossing_to_get_there[a.first][a.second + 1] = current_cross + used_times;
				}
				else {
					crossing_to_get_there[a.first][a.second + 1] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first, a.second + 1));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
			if (reference_map[a.first + 1][a.second] != 0 && temp_map[a.first + 1][a.second].first > current_g + 1) {
				temp_map[a.first + 1][a.second].second.first = a.first;
				temp_map[a.first + 1][a.second].second.second = a.second;
				temp_map[a.first + 1][a.second].first = current_g + 1;
				std::vector<size_t> used_in_times_vec = used_number_of_vertex[a.first + 1][a.second];
				size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), current_g + 1);
				if (used_times != 0) {
					crossing_to_get_there[a.first + 1][a.second] = current_cross + used_times;
				}
				else {
					crossing_to_get_there[a.first + 1][a.second] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first + 1, a.second));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
			if (reference_map[a.first][a.second - 1] != 0 && temp_map[a.first][a.second - 1].first > current_g + 1) {
				temp_map[a.first][a.second - 1].second.first = a.first;
				temp_map[a.first][a.second - 1].second.second = a.second;
				temp_map[a.first][a.second - 1].first = current_g + 1;
				std::vector<size_t> used_in_times_vec = used_number_of_vertex[a.first][a.second - 1];
				size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), current_g + 1);
				if (used_times != 0) {
					crossing_to_get_there[a.first][a.second - 1] = current_cross + used_times;
				}
				else {
					crossing_to_get_there[a.first][a.second - 1] = current_cross;
				}
				priority_queue.push_back(std::make_pair(a.first, a.second - 1));
				std::sort(priority_queue.begin(), priority_queue.end(), comparator);
			}
		}

		//reproducting the path from start to finish
		if (found) {
			size_t time_step = 1;
			std::pair<size_t, size_t> current = agents[i].first;
			output_paths[i].push_back(std::make_pair(agents[i].first.first, agents[i].first.second));
			used_number_of_vertex[current.first][current.second].push_back(time_step);
			while (current.first != agents[i].second.first || current.second != agents[i].second.second) {
				time_step++;
				current = temp_map[current.first][current.second].second;
				output_paths[i].push_back(std::make_pair(current.first, current.second));
				used_number_of_vertex[current.first][current.second].push_back(time_step);
			}
		}
	}

	//check if each agent found his finis
	for (size_t a = 0; a < agents.size(); a++) {
		if (output_paths[a].size() == 0) {
			return "ERROR: some agents can't get to finish";
		}
	}

	//make all path the same length
	for (size_t a = 0; a < agents.size(); a++) {
		while(output_paths[a].size() < makespan) {
			output_paths[a].push_back(output_paths[a].back());
		}
	}

	return "OK";
}

std::string WithoutCrossingAtSameTimes::get_name() {
	return "WithoutCrossingAtSameTimes";
}

/** Manhattan distance heuristic comparison class. */
class CompareWithoutCrossingR {
public:
	std::pair<size_t, size_t> finish;

	CompareWithoutCrossingR() {}

	CompareWithoutCrossingR(std::pair<size_t, size_t> finish) :
		finish(finish)
	{}

	bool operator()(std::pair<size_t, size_t> above, std::pair<size_t, size_t> below) const {

		size_t below_h = abs((int)below.first - (int)finish.first) + abs((int)below.second - (int)finish.second);

		size_t above_h = abs((int)above.first - (int)finish.first) + abs((int)above.second - (int)finish.second);

		//how far their are from finish
		if (below_h < above_h) {
			return true;
		}
		if (below_h > above_h) {
			return false;
		}
		//they are equal in everything
		return false;
	}
};

/** Manhattan distance heuristic with conflicts comparison class. */
class CompareWithoutCrossingRConflict {
public:
	std::pair<size_t, size_t> finish;
	size_t remaining_steps;
	std::vector<std::vector<std::vector<size_t>>>* dep_used_number_of_vertex = nullptr;

	CompareWithoutCrossingRConflict() {
		dep_used_number_of_vertex = nullptr;
	}

	CompareWithoutCrossingRConflict(std::pair<size_t, size_t> finish, size_t remaining_steps, std::vector<std::vector<std::vector<size_t>>>& used_number_of_vertex) :
		finish(finish),
		remaining_steps(remaining_steps),
		dep_used_number_of_vertex(&used_number_of_vertex)
	{}

	bool operator()(std::pair<size_t, size_t> above, std::pair<size_t, size_t> below) const {
		const std::vector<std::vector<std::vector<size_t>>>& used_number_of_vertex_reference = *dep_used_number_of_vertex;

		size_t below_h = abs((int)below.first - (int)finish.first) + abs((int)below.second - (int)finish.second);

		std::vector<size_t> used_in_times_vec_below = used_number_of_vertex_reference[below.first][below.second];
		size_t below_used_times = std::count(used_in_times_vec_below.begin(), used_in_times_vec_below.end(), remaining_steps - 1);

		size_t above_h = abs((int)above.first - (int)finish.first) + abs((int)above.second - (int)finish.second);

		std::vector<size_t> used_in_times_vec_above = used_number_of_vertex_reference[above.first][above.second];
		size_t above_used_times = std::count(used_in_times_vec_above.begin(), used_in_times_vec_above.end(), remaining_steps - 1);

		//how far their are from finish
		if (below_h < above_h) {
			return true;
		}
		if (below_h > above_h) {
			return false;
		}
		//how much their are occupied
		if (below_used_times < above_used_times) {
			return true;
		}
		if (below_used_times > above_used_times) {
			return false;
		}
		//they are equal in everything
		return false;
	}
};

template <typename T>
bool IsInVector(std::vector<T>& vec, T to_find) {
	auto it { std::find(vec.begin(), vec.end(), to_find) };

	if (it != vec.end()) {
		return true;
	}

	return false;
}

/** Detects swapping conflicts for agent.
*
* @param current_time current timestep.
* @param current_position current agent position.
* @param next_position next agent position.
* @param used_number_of_vertex information about timesteps when the vertices are used.
* @param next_vertex information about next vertex used from the current vertex (Information about ussage of edges).
* @return true if swapping conflict would occur.
*/
bool IsSwapping(
	size_t current_time, 
	std::pair<size_t, size_t> current_position, 
	std::pair<size_t, size_t> next_position, 
	std::vector<std::vector<std::vector<size_t>>>& used_number_of_vertex,
	std::vector<std::vector<std::vector<std::pair<size_t, size_t>>>>& next_vertex
) {

	for (size_t i = 0; i < used_number_of_vertex[next_position.first][next_position.second].size(); i++) {
		if (used_number_of_vertex[next_position.first][next_position.second][i] == current_time && 
			next_vertex[next_position.first][next_position.second][i] == current_position) {
			
			return true;
		}
	}

	return false;
}

/** Recursive path search.
*
* @param reachable_in_time shortest time in witch each vertex can be visited by an agent.
* @param used_number_of_vertex tracks how many times a vertex is occupied in given time.
* @param next_vertex tracks how many times an edge is occupied in given time.
* @param visited tracks if the vertex vas already visited in given time.
* @param current_position current position.
* @param goal desired goal position.
* @param remaining_steps number of time steps left to stand on goal position.
* @param remaining_conflicts number of allowed conflicts left on the path.
* @param final_path agent final path (computed during recursion).
* @return true if goal was reached with 0 steps left.
*/
bool compute_path_recursive(
	std::vector<std::vector<size_t>>& reachable_in_time,
	std::vector<std::vector<std::vector<size_t>>>& used_number_of_vertex,
	std::vector<std::vector<std::vector<std::pair<size_t, size_t>>>>& next_vertex,
	std::vector<std::vector<std::vector<size_t>>>& visited,
	std::pair<size_t, size_t> current_position,
	std::pair<size_t, size_t> goal,
	size_t remaining_steps,
	size_t remaining_conflicts, 
	std::vector<std::pair<size_t, size_t>>& final_path
) {

	//stopping condition
	if (current_position.first == goal.first && current_position.second == goal.second && remaining_steps == 0) {

		final_path.push_back(current_position);
		used_number_of_vertex[current_position.first][current_position.second].push_back(0);
		next_vertex[current_position.first][current_position.second].push_back(std::make_pair(SIZE_MAX, SIZE_MAX));

		return true;
	}

	visited[current_position.first][current_position.second].push_back(remaining_steps);

	std::vector<std::pair<size_t, size_t>> priority_queue = std::vector<std::pair<size_t, size_t>>();
	std::vector<std::pair<size_t, size_t>> conflict_priority_queue = std::vector<std::pair<size_t, size_t>>();
	CompareWithoutCrossingR comparator = CompareWithoutCrossingR(goal);
	CompareWithoutCrossingRConflict conflict_comparator = CompareWithoutCrossingRConflict(goal, remaining_steps, used_number_of_vertex);

	if (remaining_steps > 0) {
		if ((reachable_in_time[current_position.first - 1][current_position.second] != 0 || std::make_pair(current_position.first - 1, current_position.second) == goal) && 
			abs((int)reachable_in_time[goal.first][goal.second] - (int)reachable_in_time[current_position.first - 1][current_position.second]) <= remaining_steps - 1 &&
			!IsInVector(visited[current_position.first - 1][current_position.second], remaining_steps - 1)) {

			std::vector<size_t> used_in_times_vec = used_number_of_vertex[current_position.first - 1][current_position.second];
			size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), remaining_steps - 1);

			if (IsSwapping(remaining_steps, current_position, std::make_pair(current_position.first - 1, current_position.second), used_number_of_vertex, next_vertex) || used_times > 0) {
				conflict_priority_queue.push_back(std::make_pair(current_position.first - 1, current_position.second));
			}
			else {
				priority_queue.push_back(std::make_pair(current_position.first - 1, current_position.second));
			}
		}
		if ((reachable_in_time[current_position.first][current_position.second + 1] != 0 || std::make_pair(current_position.first, current_position.second + 1) == goal) && 
			abs((int)reachable_in_time[goal.first][goal.second] - (int)reachable_in_time[current_position.first][current_position.second + 1]) <= remaining_steps - 1 &&
			!IsInVector(visited[current_position.first][current_position.second + 1], remaining_steps - 1)) {

			std::vector<size_t> used_in_times_vec = used_number_of_vertex[current_position.first][current_position.second + 1];
			size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), remaining_steps - 1);

			if (IsSwapping(remaining_steps, current_position, std::make_pair(current_position.first, current_position.second + 1), used_number_of_vertex, next_vertex) || used_times > 0) {
				conflict_priority_queue.push_back(std::make_pair(current_position.first, current_position.second + 1));
			}
			else {
				priority_queue.push_back(std::make_pair(current_position.first, current_position.second + 1));
			}
		}
		if ((reachable_in_time[current_position.first + 1][current_position.second] != 0 || std::make_pair(current_position.first + 1, current_position.second) == goal) && 
			abs((int)reachable_in_time[goal.first][goal.second] - (int)reachable_in_time[current_position.first + 1][current_position.second]) <= remaining_steps - 1 &&
			!IsInVector(visited[current_position.first + 1][current_position.second], remaining_steps - 1)) {

			std::vector<size_t> used_in_times_vec = used_number_of_vertex[current_position.first + 1][current_position.second];
			size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), remaining_steps - 1);

			if (IsSwapping(remaining_steps, current_position, std::make_pair(current_position.first + 1, current_position.second), used_number_of_vertex, next_vertex) || used_times > 0) {
				conflict_priority_queue.push_back(std::make_pair(current_position.first + 1, current_position.second));
			}
			else {
				priority_queue.push_back(std::make_pair(current_position.first + 1, current_position.second));
			}
		}
		if ((reachable_in_time[current_position.first][current_position.second - 1] != 0 || std::make_pair(current_position.first, current_position.second - 1) == goal) && 
			abs((int)reachable_in_time[goal.first][goal.second] - (int)reachable_in_time[current_position.first][current_position.second - 1]) <= remaining_steps - 1 &&
			!IsInVector(visited[current_position.first][current_position.second - 1], remaining_steps - 1)) {

			std::vector<size_t> used_in_times_vec = used_number_of_vertex[current_position.first][current_position.second - 1];
			size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), remaining_steps - 1);

			if (IsSwapping(remaining_steps, current_position, std::make_pair(current_position.first, current_position.second - 1), used_number_of_vertex, next_vertex) || used_times > 0) {
				conflict_priority_queue.push_back(std::make_pair(current_position.first, current_position.second - 1));
			}
			else {
				priority_queue.push_back(std::make_pair(current_position.first, current_position.second - 1));
			}
		}

		//stand still
		if ((reachable_in_time[current_position.first][current_position.second] != 0 || std::make_pair(current_position.first, current_position.second) == goal) && 
			abs((int)reachable_in_time[goal.first][goal.second] - (int)reachable_in_time[current_position.first][current_position.second]) <= remaining_steps - 1 &&
			!IsInVector(visited[current_position.first][current_position.second], remaining_steps - 1)) {

			std::vector<size_t> used_in_times_vec = used_number_of_vertex[current_position.first][current_position.second];
			size_t used_times = std::count(used_in_times_vec.begin(), used_in_times_vec.end(), remaining_steps - 1);
			if (used_times > 0) {
				conflict_priority_queue.push_back(std::make_pair(current_position.first, current_position.second));
			}
			else {
				priority_queue.push_back(std::make_pair(current_position.first, current_position.second));
			}
		}

		std::sort(priority_queue.begin(), priority_queue.end(), comparator);
		std::sort(conflict_priority_queue.begin(), conflict_priority_queue.end(), conflict_comparator);
	}

	while (!priority_queue.empty()) {
		std::pair<size_t, size_t> next_position = priority_queue.back();
		priority_queue.pop_back();

		bool found = compute_path_recursive(reachable_in_time, used_number_of_vertex, next_vertex, visited, next_position, goal, remaining_steps - 1, remaining_conflicts, final_path);

		//reconstruction of the path
		if (found) {
			final_path.push_back(current_position);

			used_number_of_vertex[current_position.first][current_position.second].push_back(remaining_steps);
			next_vertex[current_position.first][current_position.second].push_back(next_position);

			return true;
		}
	}

	if (remaining_conflicts > 0) {
		while (!conflict_priority_queue.empty()) {
			std::pair<size_t, size_t> next_conflict_position = conflict_priority_queue.back();
			conflict_priority_queue.pop_back();

			bool found = compute_path_recursive(reachable_in_time, used_number_of_vertex, next_vertex, visited, next_conflict_position, goal, remaining_steps - 1, remaining_conflicts - 1, final_path);

			//reconstruction of the path
			if (found) {
				final_path.push_back(current_position);

				used_number_of_vertex[current_position.first][current_position.second].push_back(remaining_steps);
				next_vertex[current_position.first][current_position.second].push_back(next_conflict_position);

				return true;
			}
		}
	}

	//try different branch
	return false;
}

/** Computes the shortest paths in graph for agents.
*
* Uses RPS algorithm.
*
* @param reference_map original input map.
* @param output_paths vector for writing the result for agents.
* @param agents vector of pairs containig agents start and finish coordinates in pair.
* @return error message, "OK" if everything ended right.
*/
std::string RecursivePaths::compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents, size_t makespan) {
	if (output_paths.size() != agents.size()) {
		return "ERROR: different lenghts of paths and agents\n";
	}

	output_paths.clear();
	output_paths.resize(agents.size());

	//vector of agents starts and finishes with their corresponding reachable in time map
	std::vector<std::pair<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>, std::vector<std::vector<size_t>>>> reachable_in_time;

	std::vector<std::vector<std::vector<size_t>>> used_number_of_vertex(reference_map.size(), std::vector<std::vector<size_t>>(reference_map[0].size()));
	std::vector<std::vector<std::vector<std::pair<size_t, size_t>>>> next_vertex(reference_map.size(), std::vector<std::vector<std::pair<size_t, size_t>>>(reference_map[0].size()));

	//Compute vertex reachable time for all agents
	for (size_t i = 0; i < agents.size(); i++) {
		reachable_in_time.push_back(std::make_pair(agents[i], compute_time_expanded_graph(reference_map, agents[i].first)));
	}

	//Sort agents by their shortest path length
	std::ranges::sort(reachable_in_time,
		[]
		(
			const std::pair<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>, std::vector<std::vector<size_t>>> &a, 
			const std::pair<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>, std::vector<std::vector<size_t>>> &b
		)
		{ 
    		return a.second[a.first.second.first][a.first.second.second] > b.second[b.first.second.first][b.first.second.second];
		});

	//Compute paths for all agents
	for (size_t i = 0; i < reachable_in_time.size(); i++) {

		bool found = false;

		for (size_t allowed_conflicts = 0; !found && allowed_conflicts <= makespan; allowed_conflicts++) {

			std::vector<std::vector<std::vector<size_t>>> visited(reference_map.size(), std::vector<std::vector<size_t>>(reference_map[0].size()));

			found = compute_path_recursive(
				reachable_in_time[i].second, 
				used_number_of_vertex,
				next_vertex, 
				visited, 
				reachable_in_time[i].first.first, 
				reachable_in_time[i].first.second,
				makespan,
				allowed_conflicts, 
				output_paths[i]
			);
		}

		if (!found) {
			return "ERROR: some agents can't get to finish";
		}
	}

	//TODO: output_path dont corresponds to agents by index

	//check if each agent found his finis
	for (size_t a = 0; a < agents.size(); a++) {
		if (output_paths[a].size() == 0) {
			return "ERROR: some agents can't get to finish";
		}
	}

	return "OK";
}

std::string RecursivePaths::get_name() {
	return "RecursivePaths";
}