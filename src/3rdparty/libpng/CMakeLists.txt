set(PNG_BUILD_ZLIB ON)
set(SKIP_INSTALL_ALL ON)
get_target_property(ZLIB_INCLUDE_DIRS zlibstatic INCLUDE_DIRECTORIES)
add_subdirectory(src)
