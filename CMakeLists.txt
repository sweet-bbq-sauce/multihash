cmake_minimum_required(VERSION 3.25.1)
project(multihash CXX)

include_directories(src)

#add_subdirectory(src)

set(SOURCES

    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"

)

add_executable(multihash ${SOURCES})
target_link_libraries(multihash "-lcrypto")