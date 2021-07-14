# GLOB 不包含子目录 GLOB_RECURSE 包含子目录

# core
FILE(GLOB head_core core/*.h)
FILE(GLOB source_core core/*.cpp)
FILE(GLOB head_core_algorithm core/algorithm/*.h)
FILE(GLOB source_core_algorithm core/algorithm/*.cpp)
FILE(GLOB head_core_problem core/problem/*.h)
FILE(GLOB source_core_problem core/problem/*.cpp)
FILE(GLOB head_core_problem_continuous core/problem/continuous/*.h)
FILE(GLOB source_core_problem_continuous core/problem/continuous/*.cpp)
FILE(GLOB head_core_problem_uncertainty core/problem/uncertainty/*.h)
FILE(GLOB source_core_problem_uncertainty core/problem/uncertainty/*.cpp)
FILE(GLOB head_core_record core/record/*.h)
FILE(GLOB source_core_record core/record/*.cpp)

set(group_core ${head_core} ${source_core})
set(group_core_algorithm ${head_core_algorithm} ${source_core_algorithm})
set(group_core_problem ${head_core_problem} ${source_core_problem})
set(group_core_problem_continuous ${head_core_problem_continuous} ${source_core_problem_continuous})
set(group_core_problem_uncertainty ${head_core_problem_uncertainty} ${source_core_problem_uncertainty})
set(group_core_record ${head_core_record} ${source_core_record})

source_group(core FILES ${group_core})
source_group(core\\algorithm FILES ${group_core_algorithm})
source_group(core\\problem FILES ${group_core_problem})
source_group(core\\problem\\continuous FILES ${group_core_problem_continuous})
source_group(core\\problem\\uncertainty FILES ${group_core_problem_uncertainty})
source_group(core\\record FILES ${group_core_record})

# instance
FILE(GLOB head_instance_record instance/record/*.h)
FILE(GLOB source_instance_record instance/record/*.cpp)
set(group_instance_record ${head_instance_record} ${source_instance_record})
source_group(instance\\record FILES ${group_instance_record})

# result

# run
FILE(GLOB head_run run/*.h)
FILE(GLOB source_run run/*.cpp)
set(group_run ${head_run} ${source_run})
source_group(run FILES ${group_run})

# test
FILE(GLOB head_test test/*.h)
FILE(GLOB source_test test/*.cpp)
set(group_test ${head_test} ${source_test})
source_group(test FILES ${group_test})

# utility
FILE(GLOB head_utility utility/*.h)
FILE(GLOB source_utility utility/*.cpp)
FILE(GLOB head_utility_clustering utility/clustering/*.h)
FILE(GLOB source_utility_clustering utility/clustering/*.cpp)
FILE(GLOB head_utility_CubicSpline utility/CubicSpline/*.h)
FILE(GLOB source_utility_CubicSpline utility/CubicSpline/*.cpp)
FILE(GLOB head_utility_kd-tree utility/kd-tree/*.h)
FILE(GLOB source_utility_kd-tree utility/kd-tree/*.cpp)
FILE(GLOB head_utility_mapbox utility/mapbox/*.h)
FILE(GLOB source_utility_mapbox utility/mapbox/*.cpp)
FILE(GLOB head_utility_noisy_fun utility/noisy_fun/*.h)
FILE(GLOB source_utility_noisy_fun utility/noisy_fun/*.cpp)
FILE(GLOB head_utility_nondominated_sorting utility/nondominated_sorting/*.h)
FILE(GLOB source_utility_nondominated_sorting utility/nondominated_sorting/*.cpp)
FILE(GLOB head_utility_random utility/random/*.h)
FILE(GLOB source_utility_random utility/random/*.cpp)
FILE(GLOB head_utility_typevar utility/typevar/*.h)
FILE(GLOB source_utility_typevar utility/typevar/*.cpp)

set(group_utility ${head_utility} ${source_utility})
set(group_utility_clustering ${head_utility_clustering} ${source_utility_clustering})
set(group_utility_CubicSpline ${head_utility_CubicSpline} ${source_utility_CubicSpline})
set(group_utility_kd-tree ${head_utility_kd-tree} ${source_utility_kd-tree})
set(group_utility_mapbox ${head_utility_mapbox} ${source_utility_mapbox})
set(group_utility_noisy_fun ${head_utility_noisy_fun} ${source_utility_noisy_fun})
set(group_utility_nondominated_sorting ${head_utility_nondominated_sorting} ${source_utility_nondominated_sorting})
set(group_utility_random ${head_utility_random} ${source_utility_random})
set(group_utility_typevar ${head_utility_typevar} ${source_utility_typevar})

source_group(utility FILES ${group_utility})
source_group(utility\\clustering FILES ${group_utility_clustering})
source_group(utility\\CubicSpline FILES ${group_utility_CubicSpline})
source_group(utility\\kd-tree FILES ${group_utility_kd-tree})
source_group(utility\\mapbox FILES ${group_utility_mapbox})
source_group(utility\\noisy_fun FILES ${group_utility_noisy_fun})
source_group(utility\\nondominated_sorting FILES ${group_utility_nondominated_sorting})
source_group(utility\\random FILES ${group_utility_random})
source_group(utility\\typevar FILES ${group_utility_typevar})

add_executable(OFEC_sc2 
${group_core}
${group_core_algorithm}
${group_core_problem}
${group_core_problem_continuous}
${group_core_problem_uncertainty}
${group_core_record}
${group_run}
${group_instance_record}
${group_test}
${group_utility}
${group_utility_clustering}
${group_utility_CubicSpline}
${group_utility_kd-tree}
${group_utility_mapbox}
${group_utility_noisy_fun}
${group_utility_nondominated_sorting}
${group_utility_random}
${group_utility_typevar}
)