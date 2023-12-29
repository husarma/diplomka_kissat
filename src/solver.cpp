#include "MAPF.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Wrong number of arguments!" << std::endl;
		std::cout.flush();
        return 1;
    }

    std::ifstream instance_file = std::ifstream(argv[1]);

    int time_limit = 0;
    int bonus_makespan = 0;
    int number_of_agents = 0;
    int map_height = 0;
    int map_width = 0;

    int number_of_vertices = 0;

    std::vector<std::vector<int>> map;
    std::vector<std::pair<int, int>> starts;
	std::vector<std::pair<int, int>> goals;

	if (instance_file.is_open()) {
		std::string instance_line;

		std::getline(instance_file, instance_line); //time limit in s
		time_limit = atoi(&instance_line[0]);

		std::getline(instance_file, instance_line); //bonus makespan
		bonus_makespan = atoi(&instance_line[0]);

		std::getline(instance_file, instance_line); //number of agents
		number_of_agents = atoi(&instance_line[0]);

        std::getline(instance_file, instance_line); //map height
		map_height = atoi(&instance_line[0]);

        std::getline(instance_file, instance_line); //map width
		map_width = atoi(&instance_line[0]);

        //load map
        std::getline(instance_file, instance_line); //line containing map:

		map = std::vector<std::vector<int>>(map_height, std::vector<int>(map_width, 0));
        for (int i = 0; i < map_height; i++) {
            std::getline(instance_file, instance_line);
            for (int j = 0; j < map_width; j++) {
                if (instance_line[j] == '#') {
                    map[i][j] = -1;
                }
                else {
                    number_of_vertices++;
                    map[i][j] = number_of_vertices;
                }
            }
        }

        //load agents
        std::getline(instance_file, instance_line); //line containing agents:

        for (int i = 0; i < number_of_agents; i++) {

            std::getline(instance_file, instance_line);
            std::stringstream tokenizer(instance_line);
			std::string column;
			size_t asx, asy, aex, aey;
			asx = asy = aex = aey = 0;
			tokenizer >> column;
			for (size_t j = 0; j < 4; j++, tokenizer >> column) {
				switch (j) {
				case 0:
					asx = std::stoi(column);
					break;
				case 1:
					asy = std::stoi(column);
					break;
				case 2:
					aex = std::stoi(column);
					break;
				case 3:
					aey = std::stoi(column);
					break;
				default:
					break;
				}
			}

            starts.push_back(std::make_pair<int, int>((int)asx, (int)asy));
		    goals.push_back(std::make_pair<int, int>((int)aex, (int)aey));
        }
	}

	instance_file.close();

    Instance* inst = new Instance(map, starts, goals, "scenario", "map_name");
	Logger* log = new Logger(inst, "log.log", "encoding_name");
	Pass_parallel_mks_all* solver = new Pass_parallel_mks_all("encoding_name", 1);

	solver->SetData(inst, log, time_limit, true, false);
	inst->SetAgents(number_of_agents);
	log->NewInstance(number_of_agents);

	int res = solver->Solve(number_of_agents, bonus_makespan, true);
	
	delete solver;
	delete log;
	delete inst;

    std::ofstream result_file = std::ofstream(argv[2]);

	if (result_file.is_open()) {
        if (res == -1) {
            result_file << "NO solution" << std::endl;
            result_file.close();
        }
        if (res == 0) {
            result_file << "OK" << std::endl;
            result_file.close();
        }
        if (res == 1) {
            result_file << "Timed out" << std::endl;
            result_file.close();
        }
    }
    return 0;
}