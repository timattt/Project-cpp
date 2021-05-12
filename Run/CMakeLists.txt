cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

add_executable(TIMATTT_S_PROJECTS Run.cpp Starter.cpp)
set_target_properties(TIMATTT_S_PROJECTS PROPERTIES COMPILE_FLAGS -DBUILD_ALL)

add_executable(AKINATOR Run.cpp Starter.cpp)
set_target_properties(AKINATOR PROPERTIES COMPILE_FLAGS -DBUILD_AK)

add_executable(COMPILERS Run.cpp Starter.cpp)
set_target_properties(COMPILERS PROPERTIES COMPILE_FLAGS -DBUILD_COM)

add_executable(EXPRESSIONS Run.cpp Starter.cpp)
set_target_properties(EXPRESSIONS PROPERTIES COMPILE_FLAGS -DBUILD_EXPR)

add_executable(GRAMMAR Run.cpp Starter.cpp)
set_target_properties(GRAMMAR PROPERTIES COMPILE_FLAGS -DBUILD_GRAM)

add_executable(INTERPRITATOR Run.cpp Starter.cpp)
set_target_properties(INTERPRITATOR PROPERTIES COMPILE_FLAGS -DBUILD_INTR)

add_executable(TREAP_TEST Run.cpp Starter.cpp)
set_target_properties(TREAP_TEST PROPERTIES COMPILE_FLAGS -DBUILD_TREAP)