cmake_minimum_required(VERSION 3.21)
project(D2Tools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.1.5")
set(wxWidgets_LIB_DIR "C:/wxWidgets-3.1.5/lib/gcc_lib")
set(wxWidgets_USE_LIBS)
find_package(wxWidgets 3.1 REQUIRED)

set(BUILD_TESTING OFF)
add_subdirectory(lib/cryptopp)

if(wxWidgets_FOUND)
    set(HEADERS include/parse_helper.h include/package.h include/types.h include/main.h include/window.h include/file_helper.h include/extractor.h)
    set(SOURCES src/package.cpp src/main.cpp src/window_main.cpp src/window_events.cpp src/window_package_selector.cpp src/extractor.cpp)

    add_definitions(-DwxUSE_RC_MANIFEST=1)
    include("${wxWidgets_USE_FILE}")
    add_executable(D2Tools ${SOURCES} ${HEADERS})
    target_sources(D2Tools PRIVATE manifest.rc)
    target_include_directories(D2Tools PRIVATE include lib)
    target_link_libraries(D2Tools ${wxWidgets_LIBRARIES} cryptopp-static)

else(wxWidgets_FOUND)
    message(SEND_ERROR "Failed to set up project, wxWidgets not found!")
endif()


