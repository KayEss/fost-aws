cmake_minimum_required(VERSION 3.10)
project(fost-aws-circleci)

add_custom_target(check)

add_subdirectory(boost)
add_subdirectory(crypto)
add_subdirectory(cord)
add_subdirectory(fost-base)
add_subdirectory(.. fost-aws) # fost-base must be before
add_subdirectory(fost-internet)
add_subdirectory(threading)
