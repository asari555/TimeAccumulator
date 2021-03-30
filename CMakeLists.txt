cmake_minimum_required (VERSION 3.17)
project(time_accumulator)

include(FetchContent)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui)
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -pthread")

add_executable(time_accumulator main.cpp liveApplication.cpp liveApplication.h
        mainSystem.cpp mainSystem.h intermediateSystem.cpp intermediateSystem.h
        fileSystem.cpp fileSystem.h language.h language.cpp)
target_link_libraries(time_accumulator
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component)
set_target_properties(time_accumulator PROPERTIES CXX_STANDARD 20)
