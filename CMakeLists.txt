cmake_minimum_required(VERSION 3.13)

add_subdirectory("lpc")
add_subdirectory("glitcher")
project("main")

add_custom_target(AllBuild)
add_dependencies(AllBuild lpc_fw glitcher)

# include("./glitcher/CMakeLists.txt")

