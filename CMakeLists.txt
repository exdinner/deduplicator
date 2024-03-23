cmake_minimum_required(VERSION 3.8)
project(
  deduplicator
  LANGUAGES CXX
  VERSION 1.0.0
  DESCRIPTION "A hash and file size based de-duplication tool"
)

find_package(sqlitemm REQUIRED CONFIG)
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENSSL REQUIRED openssl)

set(${PROJECT_NAME}_INCLUDES
  ${PROJECT_SOURCE_DIR}/include
)

set(${PROJECT_NAME}_SRCS
  ${PROJECT_SOURCE_DIR}/src/context.cpp
  ${PROJECT_SOURCE_DIR}/src/file_status.cpp
  ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${PROJECT_SOURCE_DIR}/src/misc.cpp
  ${PROJECT_SOURCE_DIR}/src/util.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE sqlitemm::sqlitemm)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARIES})
