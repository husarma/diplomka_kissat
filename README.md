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

Create `<Tests folder name>-Results.xlsx` file containing results from experiments by running python script located in `Experiments` folder by `python Experiments/results_maker.py --data_path Experiments/<Tests folder name>`. You can create conda enviroment with all required libraries with `env/environment.yml` file after miniconda [instalation](https://docs.anaconda.com/miniconda/install/) with `conda env create -f environment.yml` command.

Example of runing experiemnts `./build/mapf_experiments -mp -wx 30 Experiments/Tests`

Example of generating `Tests-Results.xlsx` results file `python Experiments/results_maker.py --data_path Experiments/Tests`


Be aware of high memory requirements for large maps or high agents count.

Big thanks to https://github.com/svancaj/MAPF-encodings for MAPF solving library based on kissat SAT solver. (`MAPF.hpp`, `libmapf.a`)

For the tasks management and execution the [BS-thread-pool](https://github.com/bshoshany/thread-pool) library was used.
