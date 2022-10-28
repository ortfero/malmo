project('malmo', 'cpp',
    license: 'MIT',
    default_options: [
        'cpp_std=c++17',
        'warning_level=3'])

malmo = declare_dependency(
    include_directories: include_directories('./include'),
    sources: [
        'include/malmo/pyramid.hpp'
    ]
)

subdir('test')
subdir('benchmark')
