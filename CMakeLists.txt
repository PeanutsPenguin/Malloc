
cmake_minimum_required(VERSION 3.22.1)

project(directories)

# Define variables for source and include directories
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the include directory to the search path for headers
include_directories(${INCLUDE_DIR})

file(
    #GLOB_RECURSE: This specifies the mode in which the file command should operate. GLOB_RECURSE is used 
    #to search for files recursively in directories that match the provided patterns.
    # It will look for files in subdirectories as well.

    # PROJECT_FILES is the name of the variable where the list of files matching the specified patterns 
    #will be stored.
    GLOB_RECURSE PROJECT_FILES
    ${INCLUDE_DIR}/*.h
    ${SOURCE_DIR}/*.c
    )


add_compile_options(-g -Wextra -Wall -Wpedantic)



add_executable(exe)
    
#target_sources command in CMake is used to specify source files that should be associated with a 
#particular target
# (usually a binary executable or a library) in the project.

#PRIVATE keyword indicates that these source files are private to the exe target,
# meaning they won't be visible to other targets that depend on exe.
target_sources(exe PRIVATE ${PROJECT_FILES})

target_include_directories(exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(exe PRIVATE m)
