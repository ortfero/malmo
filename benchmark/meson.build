abseil = declare_dependency(
    include_directories: include_directories('../thirdparty/include')
)

malmo_benchmark = executable('malmo-benchmark', 
    'benchmark.cpp', dependencies: [malmo, abseil])
