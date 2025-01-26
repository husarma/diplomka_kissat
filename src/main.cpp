#include "Map.hpp"
#include "Compute_Strategy.hpp"
#include "Path_finder_Strategy.hpp"

#include "BS_thread_pool.hpp" //https://github.com/bshoshany/thread-pool

#include <thread>
#include <filesystem>
#include <algorithm>

#include <sstream>
#include <cctype>
#include <cmath>

bool string_dir_comparator(const std::string& a, const std::string& b)
{
    if (a.empty())
        return true;
    if (b.empty())
        return false;
    if (std::isdigit(a[0]) && !std::isdigit(b[0]))
        return true;
    if (!std::isdigit(a[0]) && std::isdigit(b[0]))
        return false;
    if (!std::isdigit(a[0]) && !std::isdigit(b[0]))
    {
        if (std::toupper(a[0]) == std::toupper(b[0]))
            return string_dir_comparator(a.substr(1), b.substr(1));
        return (std::toupper(a[0]) < std::toupper(b[0]));
    }

    // Both strings begin with digit --> parse both numbers
    std::istringstream issa(a);
    std::istringstream issb(b);
    int ia, ib;
    issa >> ia;
    issb >> ib;
    if (ia != ib)
        return ia < ib;

    // Numbers are the same --> remove numbers and recurse
    std::string anew, bnew;
    std::getline(issa, anew);
    std::getline(issb, bnew);
    return (string_dir_comparator(anew, bnew));
}

std::unique_ptr<PathFinderI> create_path_finder_strategy(char p) {
    switch (p)
    {
    case 'b':
        return std::make_unique<Biaset>();
    case 'r':
        return std::make_unique<TrullyRandom>();
    case 'w':
        return std::make_unique<WithoutCrossing>();
    case 'x':
        return std::make_unique<WithoutCrossingAtSameTimes>();
    case 'R':
        return std::make_unique<RecursivePaths>();
    default:
        std::cout << "ERROR: Undefined path finder option: -" << p << std::endl;
        throw std::invalid_argument("ERROR: Undefined path finder option: -" + p);
        break;
    }
}

void runbase(std::string a, std::string b, char p, std::string c, size_t t) {

    std::unique_ptr<ComputeStrategyI> base = std::make_unique<Baseline>();

    base->set_map_file(a);
    base->set_agents_dir(b);
    base->set_output_dir(c);
    base->set_path_finder(create_path_finder_strategy(p));
    base->run_tests(t*1000);
}

void runmake(std::string a, std::string b, char p, std::string c, size_t t) {

    std::unique_ptr<ComputeStrategyI> make = std::make_unique<MakespanAdd>();

    make->set_map_file(a);
    make->set_agents_dir(b);
    make->set_output_dir(c);
    make->set_path_finder(create_path_finder_strategy(p));
    make->run_tests(t*1000);
}

void runprun(std::string a, std::string b, char p, std::string c, size_t t) {

    std::unique_ptr<ComputeStrategyI> prun = std::make_unique<PruningCut>();

    prun->set_map_file(a);
    prun->set_agents_dir(b);
    prun->set_output_dir(c);
    prun->set_path_finder(create_path_finder_strategy(p));
    prun->run_tests(t*1000);
}

void runcomb(std::string a, std::string b, char p, std::string c, size_t t) {

    std::unique_ptr<ComputeStrategyI> comb = std::make_unique<Combined>();

    comb->set_map_file(a);
    comb->set_agents_dir(b);
    comb->set_output_dir(c);
    comb->set_path_finder(create_path_finder_strategy(p));
    comb->run_tests(t*1000);
}

int main(int argc, char** argv) {

    std::vector<std::string> dirs;
    double tasks_total = 0;

    if (argc != 5) {
        std::cout << "ERROR: Wrong number of arguments" << std::endl;
        return 1;
    }

    size_t time_limit_s = atoi(argv[3]);

    BS::thread_pool thread_pool(1);

    for (auto& p : std::filesystem::directory_iterator(argv[4])) {

        dirs.push_back(p.path().string());
    }

    std::sort(dirs.begin(), dirs.end(), string_dir_comparator);

    //Full thread_pool with tasks to compute
    for (size_t i = 0; i < dirs.size(); i++) {
        for (auto& p : std::filesystem::directory_iterator(dirs[i])) {

            std::string map_name = p.path().string();
            if (map_name.substr(map_name.size() - 4) == ".map") {
                std::cout << map_name << std::endl;

                std::string alg_params = argv[1];
                if (alg_params[0] != '-') {
                    std::cout << "ERROR: Wrong arguments format" << std::endl;
                    return 1;
                }
                std::string path_finder_params = argv[2];
                if (path_finder_params[0] != '-') {
                    std::cout << "ERROR: Wrong arguments format" << std::endl;
                    return 1;
                }
                for (size_t j = 1; j < alg_params.size(); j++) {

                    std::string agents_dir = dirs[i] + "/scen-even";

                    for (size_t p = 1; p < path_finder_params.size(); p++) {

                        tasks_total++;

                        switch (alg_params[j])
                        {
                            case !('b' || 'm' || 'p' || 'c') :
                                std::cout << "ERROR: Undefined algorithm option: -" << alg_params[j] << std::endl;
                                return 1;
                            case 'b':
                                thread_pool.push_task(runbase, map_name, agents_dir, path_finder_params[p], dirs[i], time_limit_s);
                                break;
                            case 'm':
                                thread_pool.push_task(runmake, map_name, agents_dir, path_finder_params[p], dirs[i], time_limit_s);
                                break;
                            case 'p':
                                thread_pool.push_task(runprun, map_name, agents_dir, path_finder_params[p], dirs[i], time_limit_s);
                                break;
                            case 'c':
                                thread_pool.push_task(runcomb, map_name, agents_dir, path_finder_params[p], dirs[i], time_limit_s);
                                break;
                            default:
                                //Should not occur
                                return 1;
                        }
                    }  
                }
            }
        }
    }

    //Wait for all tasks to be done
    size_t elapsed_min = 0;
    auto tasks_left = thread_pool.get_tasks_total();
    while (tasks_left != 0) {
        
        tasks_left = thread_pool.get_tasks_total();
        std::cout << std::ceil((tasks_total - tasks_left) / tasks_total * 100 * 100.0) / 100.0 << "% <-> " << tasks_total - tasks_left << "/" << tasks_total << " Tasks done. Run time: " << elapsed_min/60 << ":" << elapsed_min%60 << " h:m" << std::endl;
        if (tasks_left != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(60000));
        }
        elapsed_min++;
    }

    std::cout << "ALL JOBS DONE!" << std::endl;

    std::filesystem::remove_all("temp");

    return 0;
}