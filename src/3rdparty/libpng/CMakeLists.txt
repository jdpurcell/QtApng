set(SKIP_INSTALL_ALL ON)
set(ZLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../zlib/src")
find_package(ZLIB REQUIRED)
add_subdirectory(src)
