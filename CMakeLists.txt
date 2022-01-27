cmake_minimum_required(VERSION 3.21)
project(D2Tools)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(wxWidgets_ROOT_DIR D:/Development/Libs/wxWidgets-3.1.5)
set(wxWidgets_LIB_DIR D:/Development/Libs/wxWidgets-3.1.5/lib/gcc_lib)
set(wxWidgets_USE_LIBS)

find_package(wxWidgets 3.1 REQUIRED)
find_package(OpenSSL REQUIRED Crypto)

if(wxWidgets_FOUND)
    if(OPENSSL_FOUND)
        set(HEADERS include/parse_helper.h include/package.h include/types.h include/main.h include/window.h include/file_helper.h include/extractor.h include/callbacks.h)
        set(SOURCES src/package.cpp src/main.cpp src/window_main.cpp src/window_events.cpp src/window_package_selector.cpp src/extractor.cpp)

        set(LINK_LIBS ${wxWidgets_LIBRARIES} OpenSSL::Crypto)
        set(INCLUDES include lib ${OPENSSL_INCLUDE_DIR})

        add_executable(D2Tools ${SOURCES} ${HEADERS})
        add_definitions(-DwxUSE_RC_MANIFEST=1)
        include("${wxWidgets_USE_FILE}")
        target_include_directories(D2Tools PRIVATE ${INCLUDES})
        target_link_libraries(D2Tools ${LINK_LIBS})
        target_sources(D2Tools PRIVATE manifest.rc)
    else(OPENSSL_FOUND)
        message(SEND_ERROR "Failed to set up project, OpenSSL not found!")
    endif()
else(wxWidgets_FOUND)
    message(SEND_ERROR "Failed to set up project, wxWidgets not found!")
endif()


