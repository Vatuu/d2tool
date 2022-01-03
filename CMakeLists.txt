cmake_minimum_required(VERSION 3.21)
project(D2Tools)

set(CMAKE_CXX_STANDARD 23)

set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.1.5")
set(wxWidgets_USE_LIBS)
find_package(wxWidgets REQUIRED)

if(wxWidgets_FOUND)
    set(HEADERS include/parse_helper.h include/package.h include/types.h include/main.h include/window_extractor.h)
    set(SOURCES src/package.cpp src/main.cpp src/window_extractor.cpp)

    include("${wxWidgets_USE_FILE}")
    add_executable(D2Tools ${SOURCES} ${HEADERS})
    target_include_directories(D2Tools PRIVATE include)
    target_link_libraries(D2Tools ${wxWidgets_LIBRARIES})
else(wxWidgets_FOUND)
    message(SEND_ERROR "Failed to set up project, wxWidgets not found!")
endif()


