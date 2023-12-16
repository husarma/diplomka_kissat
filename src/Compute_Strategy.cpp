#include "Compute_Strategy.hpp"

#include "Debug.hpp"

#include <filesystem>
#include <chrono>
#include <cmath>

/** Setting path to map.
*
* @param path file containing map.
*/
void ComputeStrategyI::set_map_file(std::string path) {
	map_path = path;
}

/** Setting path to agents.
*
* @param path directory containing agents.
*/
void ComputeStrategyI::set_agents_dir(std::string path) {
	agents_dir_path = path;
}

/** Setting path to output directory.
*
* @param path directory for generating output to.
*/
void ComputeStrategyI::set_output_dir(std::string path) {
	output_dir_path = path;
}

/** Setting shortest paths finder strategy.
*
* @param p path finder strategy.
*/
void ComputeStrategyI::set_path_finder(std::unique_ptr<PathFinderI> p) {
	path_finder = std::move(p);
}

//////////////////////////////////////// Kissat usage ////////////////////////////////////////

/** Execute concrete computing algorithm for finding solution.*/
std::string Baseline::run_tests(size_t time_limit) {

	std::vector<std::string> agents_paths;

	for (const auto& entry : std::filesystem::directory_iterator(agents_dir_path)) {
		agents_paths.push_back(entry.path().string());
	}

	Map mapa;
	mapa.set_map_file(map_path);
	std::string err = mapa.load_map();
	if (err != "OK") {
		std::cout << err << std::endl;
		return err;
	}
	mapa.expanded_map = mapa.map;

	for (size_t i = 0; i < agents_paths.size(); i++) {

		int number_of_agents_to_compute = 0;
		size_t LB = 0;
		size_t bonus_makespan = 0;
		mapa.set_agents_file(agents_paths[i]);
		std::string result = "OK";

		std::chrono::time_point<std::chrono::high_resolution_clock> started;
		long long preprocess_time_total = 0;
		long long solver_time_total = 0;

		while (result != "Timed out") {

			if (result == "OK") {

				started = std::chrono::high_resolution_clock::now();
				preprocess_time_total = 0;
				solver_time_total = 0;

				bonus_makespan = 0;
				number_of_agents_to_compute += 5;

				err = mapa.load_agents(number_of_agents_to_compute);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}

				err = path_finder->compute_shortest_paths(mapa.map, mapa.agents_shortest_paths, mapa.agents);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}

				LB = mapa.get_min_time();

				auto shortest_paths_time_end = std::chrono::high_resolution_clock::now();
				preprocess_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(shortest_paths_time_end - started).count();
			}

			if (result == "NO solution") {
				bonus_makespan++;
			}

			auto time_mark = std::chrono::high_resolution_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_mark - started).count();

			// Start solver
			result = mapa.kissat(output_dir_path + "/B_" + path_finder->get_name() + "_log.txt", "B_" + path_finder->get_name(), LB + bonus_makespan, std::max((long long)1000, (long long)time_limit - elapsed_ms));

			auto solver_time_end = std::chrono::high_resolution_clock::now();
			solver_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(solver_time_end - time_mark).count();
			
			if (result == "OK") {

				auto done = std::chrono::high_resolution_clock::now();
				long long elapsed_time_total = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
				std::ofstream ofile;
				ofile.open(output_dir_path + "/B_" + path_finder->get_name() + "_results.txt", std::ios::app);

				if (ofile.is_open()) {

					std::string table_row =
						mapa.map_file_name.substr(mapa.map_file_name.find_last_of("/") + 1)
						+ "\t" +
						mapa.agents_file_name.substr(mapa.agents_file_name.find_last_of("/") + 1)
						+ "\t" +
						std::to_string(mapa.original_number_of_vertices)
						+ "\t" +
						std::to_string(number_of_agents_to_compute)
						+ "\t" +
						std::to_string(LB)
						+ "\t" +
						std::to_string(LB + bonus_makespan)
						+ "\t" +
						std::to_string(preprocess_time_total)
						+ "\t" +
						std::to_string(solver_time_total)
						+ "\t" +
						std::to_string(elapsed_time_total);
					ofile << table_row << std::endl;
				}

				ofile.close();
			}
		}
	}

	return "OK";
}

/** Execute concrete computing algorithm for finding solution.*/
std::string MakespanAdd::run_tests(size_t time_limit) {

	std::vector<std::string> agents_paths;

	for (const auto& entry : std::filesystem::directory_iterator(agents_dir_path)) {
		agents_paths.push_back(entry.path().string());
	}

	Map mapa;
	mapa.set_map_file(map_path);
	std::string err = mapa.load_map();
	if (err != "OK") {
		std::cout << err << std::endl;
		return err;
	}

	for (size_t i = 0; i < agents_paths.size(); i++) {

		int number_of_agents_to_compute = 0;
		size_t LB = 0;
		size_t bonus_makespan = 0;
		size_t current_usend_number_of_vertices = 0;
		mapa.set_agents_file(agents_paths[i]);
		std::string result = "OK";

		std::chrono::time_point<std::chrono::high_resolution_clock> started;
		long long preprocess_time_total = 0;
		long long solver_time_total = 0;

		while (result != "Timed out") {

			if (result == "OK") {

				started = std::chrono::high_resolution_clock::now();
				preprocess_time_total = 0;
				solver_time_total = 0;

				bonus_makespan = 0;
				number_of_agents_to_compute += 5;

				err = mapa.load_agents(number_of_agents_to_compute);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}

				err = path_finder->compute_shortest_paths(mapa.map, mapa.agents_shortest_paths, mapa.agents);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				LB = mapa.get_min_time();
				auto [reduced, reduced_number_of_vertices, reduced_map] = cut_unreachable(mapa.map, mapa.agents, LB);

				mapa.reset_computed_map();
				paths_to_map(mapa.agents_shortest_paths, mapa.expanded_map);

				err = expand_map(reduced_map, mapa.expanded_map, mapa.expanded_map).first;
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);

				auto shortest_paths_time_end = std::chrono::high_resolution_clock::now();
				preprocess_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(shortest_paths_time_end - started).count();
			}

			if (result == "NO solution") {
				auto bonus_makespan_time_start = std::chrono::high_resolution_clock::now();
				bonus_makespan++;
				auto [reduced, reduced_number_of_vertices, reduced_map] = cut_unreachable(mapa.map, mapa.agents, LB + bonus_makespan);

				mapa.reset_computed_map();
				paths_to_map(mapa.agents_shortest_paths, mapa.expanded_map);

				err = expand_map(reduced_map, mapa.expanded_map, mapa.expanded_map).first;
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);

				auto bonus_makespan_time_end = std::chrono::high_resolution_clock::now();
				preprocess_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(bonus_makespan_time_end - bonus_makespan_time_start).count();
			}

			auto time_mark = std::chrono::high_resolution_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_mark - started).count();

			// Start solver
			result = mapa.kissat(output_dir_path + "/M_" + path_finder->get_name() + "_log.txt", "M_" + path_finder->get_name(), LB + bonus_makespan, std::max((long long)1000, (long long)time_limit - elapsed_ms));

			auto solver_time_end = std::chrono::high_resolution_clock::now();
			solver_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(solver_time_end - time_mark).count();
			
			if (result == "OK") {

				auto done = std::chrono::high_resolution_clock::now();
				long long elapsed_time_total = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
				std::ofstream ofile;
				ofile.open(output_dir_path + "/M_" + path_finder->get_name() + "_results.txt", std::ios::app);

				if (ofile.is_open()) {

					std::string table_row =
						mapa.map_file_name.substr(mapa.map_file_name.find_last_of("/") + 1)
						+"\t" +
						mapa.agents_file_name.substr(mapa.agents_file_name.find_last_of("/") + 1)
						+"\t" +
						std::to_string(current_usend_number_of_vertices)
						+ "\t" +
						std::to_string(number_of_agents_to_compute)
						+ "\t" +
						std::to_string(LB)
						+ "\t" +
						std::to_string(LB + bonus_makespan)
						+ "\t" +
						std::to_string(preprocess_time_total)
						+ "\t" +
						std::to_string(solver_time_total)
						+ "\t" +
						std::to_string(elapsed_time_total);
					ofile << table_row << std::endl;
				}

				ofile.close();
			}
		}
	}

	return "OK";
}

/** Execute concrete computing algorithm for finding solution.*/
std::string PruningCut::run_tests(size_t time_limit) {

	std::vector<std::string> agents_paths;

	for (const auto& entry : std::filesystem::directory_iterator(agents_dir_path)) {
		agents_paths.push_back(entry.path().string());
	}

	Map mapa;
	mapa.set_map_file(map_path);
	std::string err = mapa.load_map();
	if (err != "OK") {
		std::cout << err << std::endl;
		return err;
	}

	for (size_t i = 0; i < agents_paths.size(); i++) {

		int number_of_agents_to_compute = 0;
		size_t LB = 0;
		size_t bonus_makespan = 0;
		double k = 1;
		size_t current_usend_number_of_vertices = 0;
		std::vector<std::vector<size_t>> reduced_map;
		bool try_full_map = false;
		mapa.set_agents_file(agents_paths[i]);
		std::string result = "OK";

		std::chrono::time_point<std::chrono::high_resolution_clock> started;
		long long preprocess_time_total = 0;
		long long solver_time_total = 0;

		while (result != "Timed out") {

			if (result == "OK") {

				started = std::chrono::high_resolution_clock::now();
				preprocess_time_total = 0;
				solver_time_total = 0;

				bonus_makespan = 0;
				k = 1;
				try_full_map = false;
				number_of_agents_to_compute += 5;

				err = mapa.load_agents(number_of_agents_to_compute);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}

				err = path_finder->compute_shortest_paths(mapa.map, mapa.agents_shortest_paths, mapa.agents);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				LB = mapa.get_min_time();
				auto [reduced, reduced_number_of_vertices, red_map] = cut_unreachable(mapa.map, mapa.agents, LB);
				reduced_map = red_map;

				mapa.reset_computed_map();
				paths_to_map(mapa.agents_shortest_paths, mapa.expanded_map);

				current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);

				auto shortest_paths_time_end = std::chrono::high_resolution_clock::now();
				preprocess_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(shortest_paths_time_end - started).count();
			}

			if (result == "NO solution") {
				std::pair<std::string, bool> res = std::make_pair("OK", true);
				for (size_t j = 0; j < k; j++) {
					if (try_full_map && j == 0) {
						try_full_map = false;
						res.second = false;
						break;
					}
					res = expand_map(reduced_map, mapa.expanded_map, mapa.expanded_map);
					if (!res.second && j != 0) {
						try_full_map = true;
					}
					current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);
				}

				if (k == 1) {
					k += 1;
				}
				else {
					k = pow(k, 2);
				}

				err = res.first;
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				if (!res.second) { //if not expanded then add makespan and make max pruning
					if (try_full_map == false) {
						bonus_makespan++;
						auto [reduced, reduced_number_of_vertices, red_map] = cut_unreachable(mapa.map, mapa.agents, LB + bonus_makespan);
						reduced_map = red_map;
						k = 1;
						mapa.reset_computed_map();
						paths_to_map(mapa.agents_shortest_paths, mapa.expanded_map);
						current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);
					}
				}
			}

			auto time_mark = std::chrono::high_resolution_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_mark - started).count();

			// Start solver
			result = mapa.kissat(output_dir_path + "/P_" + path_finder->get_name() + "_log.txt", "P_" + path_finder->get_name(), LB + bonus_makespan, std::max((long long)1000, (long long)time_limit - elapsed_ms));

			auto solver_time_end = std::chrono::high_resolution_clock::now();
			solver_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(solver_time_end - time_mark).count();
			
			if (result == "OK") {

				auto done = std::chrono::high_resolution_clock::now();
				long long elapsed_time_total = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
				std::ofstream ofile;
				ofile.open(output_dir_path + "/P_" + path_finder->get_name() + "_results.txt", std::ios::app);

				if (ofile.is_open()) {

					std::string table_row =
						mapa.map_file_name.substr(mapa.map_file_name.find_last_of("/") + 1)
						+"\t" +
						mapa.agents_file_name.substr(mapa.agents_file_name.find_last_of("/") + 1)
						+"\t" +
						std::to_string(current_usend_number_of_vertices)
						+ "\t" +
						std::to_string(number_of_agents_to_compute)
						+ "\t" +
						std::to_string(LB)
						+ "\t" +
						std::to_string(LB + bonus_makespan)
						+ "\t" +
						std::to_string(preprocess_time_total)
						+ "\t" +
						std::to_string(solver_time_total)
						+ "\t" +
						std::to_string(elapsed_time_total);
					ofile << table_row << std::endl;
				}

				ofile.close();
			}
		}
	}

	return "OK";
}

/** Execute concrete computing algorithm for finding solution.*/
std::string Combined::run_tests(size_t time_limit) {

	std::vector<std::string> agents_paths;

	for (const auto& entry : std::filesystem::directory_iterator(agents_dir_path)) {
		agents_paths.push_back(entry.path().string());
	}

	Map mapa;
	mapa.set_map_file(map_path);
	std::string err = mapa.load_map();
	if (err != "OK") {
		std::cout << err << std::endl;
		return err;
	}

	for (size_t i = 0; i < agents_paths.size(); i++) {

		int number_of_agents_to_compute = 0;
		size_t LB = 0;
		size_t bonus_makespan = 0;
		size_t current_usend_number_of_vertices = 0;
		mapa.set_agents_file(agents_paths[i]);
		std::string result = "OK";

		std::chrono::time_point<std::chrono::high_resolution_clock> started;
		long long preprocess_time_total = 0;
		long long solver_time_total = 0;

		while (result != "Timed out") {

			if (result == "OK") {

				started = std::chrono::high_resolution_clock::now();
				preprocess_time_total = 0;
				solver_time_total = 0;

				bonus_makespan = 0;
				number_of_agents_to_compute += 5;

				err = mapa.load_agents(number_of_agents_to_compute);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}

				err = path_finder->compute_shortest_paths(mapa.map, mapa.agents_shortest_paths, mapa.agents);
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				mapa.reset_computed_map();
				paths_to_map(mapa.agents_shortest_paths, mapa.expanded_map);

				current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);

				LB = mapa.get_min_time();

				auto shortest_paths_time_end = std::chrono::high_resolution_clock::now();
				preprocess_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(shortest_paths_time_end - started).count();
			}

			if (result == "NO solution") {
				auto bonus_makespan_time_start = std::chrono::high_resolution_clock::now();
				bonus_makespan++;
				auto [reduced, reduced_number_of_vertices, reduced_map] = cut_unreachable(mapa.map, mapa.agents, LB + bonus_makespan);

				err = expand_map(reduced_map, mapa.expanded_map, mapa.expanded_map).first;
				if (err != "OK") {
					std::cout << err << std::endl;
					return err;
				}
				current_usend_number_of_vertices = give_new_numbering(mapa.expanded_map);

				auto bonus_makespan_time_end = std::chrono::high_resolution_clock::now();
				preprocess_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(bonus_makespan_time_end - bonus_makespan_time_start).count();
			}

			auto time_mark = std::chrono::high_resolution_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_mark - started).count();

			// Start solver
			result = mapa.kissat(output_dir_path + "/C_" + path_finder->get_name() + "_log.txt", "C_" + path_finder->get_name(), LB + bonus_makespan, std::max((long long)1000, (long long)time_limit - elapsed_ms));

			auto solver_time_end = std::chrono::high_resolution_clock::now();
			solver_time_total += std::chrono::duration_cast<std::chrono::milliseconds>(solver_time_end - time_mark).count();
			
			if (result == "OK") {

				auto done = std::chrono::high_resolution_clock::now();
				long long elapsed_time_total = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();
				std::ofstream ofile;
				ofile.open(output_dir_path + "/C_" + path_finder->get_name() + "_results.txt", std::ios::app);

				if (ofile.is_open()) {

					std::string table_row =
						mapa.map_file_name.substr(mapa.map_file_name.find_last_of("/") + 1)
						+"\t" +
						mapa.agents_file_name.substr(mapa.agents_file_name.find_last_of("/") + 1)
						+"\t" +
						std::to_string(current_usend_number_of_vertices)
						+ "\t" +
						std::to_string(number_of_agents_to_compute)
						+ "\t" +
						std::to_string(LB)
						+ "\t" +
						std::to_string(LB + bonus_makespan)
						+ "\t" +
						std::to_string(preprocess_time_total)
						+ "\t" +
						std::to_string(solver_time_total)
						+ "\t" +
						std::to_string(elapsed_time_total);
					ofile << table_row << std::endl;
				}

				ofile.close();
			}
		}
	}

	return "OK";
}
