cmake_minimum_required(VERSION 3.2)
project(NoiseSaver)
add_definitions(-w)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#add_definitions(--verbose)
add_definitions(-I)
add_definitions(-DGLEW_STATIC)
add_definitions(-_MBCS)



add_executable(Noise WIN32 Noise/NoiseSaver.cpp
                     Noise/glew.c
                     Noise/glew.h
                     Noise/glxew.h
                     Noise/wglew.h
                     Noise/Renderer.cpp
                     Noise/Window.cpp
                     )

add_definitions(-mwindows)
set (EXTRA_LIBS "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" "opengl32.lib")
target_link_libraries (Noise ${EXTRA_LIBS})
