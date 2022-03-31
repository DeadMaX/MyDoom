# CMakeList.txt : projet CMake pour WebAssembloom, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Ajoutez une source à l'exécutable de ce projet.
add_executable (WebAssembloom
"WebAssembloom.cpp"
"mainwindow.ui"
"mainwindow.cpp"
"m_fixed.cpp"
"d_main.cpp"
"w_wad.cpp"
"r_main.cpp"
"tables.cpp"
"d_net.cpp"
"i_system.cpp"
"v_video.cpp"
"i_video.cpp"
"g_game.cpp"
"r_data.cpp"
"p_setup.cpp" 
"f_wipe.cpp")

target_link_libraries(WebAssembloom PUBLIC udp Qt6::Widgets)

# target_link_options(WebAssembloom PRIVATE "--no-heap-copy" "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/fonts@/fonts")

# TODO: Ajoutez des tests et installez des cibles si nécessaire.
