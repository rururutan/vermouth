#=====================================================================
# CMakeLists.txt for Vermouth
#=====================================================================
cmake_minimum_required(VERSION 3.20)

#------------------------------------------
# Porject setting
#------------------------------------------
project(vermouth LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#------------------------------------------
# Source
#------------------------------------------
file(GLOB_RECURSE SRC_CPP RELATIVE ${PROJECT_SOURCE_DIR} ./src/Win9x/*.cpp)
file(GLOB_RECURSE SRC_C RELATIVE ${PROJECT_SOURCE_DIR} ./src/*.c)

#------------------------------------------
# Make DLL file
#------------------------------------------
add_library(vermouth SHARED ${SRC_C} ${SRC_CPP} )

#------------------------------------------
# Compile Option
#------------------------------------------
target_compile_options(vermouth PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Release>:/MT /W3 /EHsc>
        $<$<CONFIG:Debug>:/MDd /W3 /EHsc>
    >
)

target_include_directories(vermouth PRIVATE ./src/ ./src/codecnv ./src/common ./src/generic ./src/vermouth ./src/Win9x ./src/zlib)

target_compile_definitions(vermouth PRIVATE
    UNICODE
    _UNICODE
    _USRDLL
    WINVER=0x0601
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    >
)

#------------------------------------------
# Link
#------------------------------------------
target_link_libraries(vermouth PRIVATE
    user32
    gdi32
    shlwapi
    winmm
)

#------------------------------------------
# Output folder
#------------------------------------------
set_target_properties(vermouth PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

