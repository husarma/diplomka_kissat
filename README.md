- compile by `make`

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

- run experiments by `./build/mapf_experiments -{b|m|p|c} -{b|r|w|x} {number of threads} {relative path to tests folder}`
- example of runing experiemnts `./build/mapf_experiments -mp -wx 4 Experiments/Tests`
