cmake_minimum_required(VERSION 3.5)

project(EntropyReducer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(LZMA_INCLUDE_DIR lzma.h
          PATHS /usr/include
          NO_DEFAULT_PATH)

find_library(LZMA_LIBRARY lzma
             PATHS /usr/lib/x86_64-linux-gnu/
             NO_DEFAULT_PATH)

if(NOT LZMA_INCLUDE_DIR OR NOT LZMA_LIBRARY)
    message(FATAL_ERROR "lzma kütüphanesi veya başlık dosyaları bulunamadı.")
endif()

set(SOURCES
    compressionalgorithms.cpp
    transformationalgorithms.cpp
    main.cpp
)

set(HEADERS
    compressionalgorithms.h
    transformationalgorithms.h
)

add_executable(EntropyReducer ${SOURCES} ${HEADERS})

target_include_directories(EntropyReducer PRIVATE ${LZMA_INCLUDE_DIR})

target_link_libraries(EntropyReducer PRIVATE ${LZMA_LIBRARY})

install(TARGETS EntropyReducer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


