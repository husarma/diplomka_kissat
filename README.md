Compile by `make`

Search algorithms:
- `b` Baseline
- `m` MakespanAdd
- `p` PruneAndCut
- `c` Combined

Path finder algorithms:
- `b` Biaset
- `r` TrullyRandom
- `w` WithoutCrossing
- `x` WithoutCrossingAtSameTimes
- `R` RecursivePaths

Run experiments by `./build/mapf_experiments -{b|m|p|c} -{b|r|w|x|R} {time limit for each instance in seconds} {relative path to tests folder}`

Example of runing experiemnts `./build/mapf_experiments -mp -wx 30 Experiments/Tests`

Create `Results.xlsx` file containing results from experiments by running python script `results_maker.py` located in `Experiments` folder. You can create conda enviroment with all required libraries with `env/environment.yml` file after miniconda [instalation](https://docs.anaconda.com/miniconda/install/) with `conda env create -f environment.yml` command.


Be aware of high memory requirements for large maps or high agents count.

Big thanks to https://github.com/svancaj/MAPF-encodings for MAPF solving library based on kissat SAT solver. (`MAPF.hpp`, `libmapf.a`)
