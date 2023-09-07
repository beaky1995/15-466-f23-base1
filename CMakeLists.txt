# This cmake file does not actually build anything. It is only for CLion to understand imports.
# Given by Jacky Sun
cmake_minimum_required(VERSION 3.21)
project(15-466-f23-base1)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ../nest-libs/windows/SDL2/include
        ../nest-libs/windows/freetype/include
        ../nest-libs/windows/glm/include
        ../nest-libs/windows/harfbuzz/include
        ../nest-libs/windows/libogg/include
        ../nest-libs/windows/libopus/include
        ../nest-libs/windows/libopusenc/include
        ../nest-libs/windows/libpng/include
        ../nest-libs/windows/opus-tools/include
        ../nest-libs/windows/opusfile/include
        ../nest-libs/windows/zlib/include
)

add_executable(15-466-f23-base1
        data_path.cpp
        data_path.hpp
        GL.cpp
        GL.hpp
        gl_compile_program.cpp
        gl_compile_program.hpp
        gl_errors.hpp
        glcorearb.h
        Load.cpp
        Load.hpp
        load_save_png.cpp
        load_save_png.hpp
        main.cpp
        Mode.cpp
        Mode.hpp
        PlayMode.cpp
        PlayMode.hpp
        PPU466.cpp
        PPU466.hpp
        read_write_chunk.hpp)
