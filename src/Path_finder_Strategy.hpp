#pragma once

#include "Compute_Strategy.hpp"

/**
* Strategy design pattern.
* Interface of strategy which computes shortest paths.
*/
class PathFinderI {
public:
    virtual ~PathFinderI() {}
    virtual std::string get_name() = 0;
    virtual std::string compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents) = 0;
};

/** Concrete implementation of strategy for computing shortest paths.*/
class Biaset : public PathFinderI {
    std::string get_name() override;
    std::string compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents) override;
};

/** Concrete implementation of strategy for computing shortest paths.*/
class TrullyRandom : public PathFinderI {
    std::string get_name() override;
    std::string compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents) override;
};

/** Concrete implementation of strategy for computing shortest paths.*/
class WithoutCrossing : public PathFinderI {
    std::string get_name() override;
    std::string compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents) override;
};

/** Concrete implementation of strategy for computing shortest paths.*/
class WithoutCrossingAtSameTimes : public PathFinderI {
    std::string get_name() override;
    std::string compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents) override;
};

/** Concrete implementation of strategy for computing shortest paths.*/
class RecursivePaths : public PathFinderI {
    std::string get_name() override;
    std::string compute_shortest_paths(std::vector<std::vector<size_t>>& reference_map, std::vector<std::vector<std::pair<size_t, size_t>>>& output_paths, std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>>>& agents) override;
};