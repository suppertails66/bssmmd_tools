add_executable(koscmp koscmp.cpp)
target_link_libraries(koscmp kosinski_shared)
add_executable(enicmp enicmp.cpp)
target_link_libraries(enicmp enigma_shared)
add_executable(nemcmp nemcmp.cpp)
target_link_libraries(nemcmp nemesis_shared)
add_executable(saxcmp saxcmp.cpp)
target_link_libraries(saxcmp saxman_shared)
