set(APNG_SOURCES apngimagehandler_p.h apngimagehandler.cpp apngimageplugin.cpp
                 apngimageplugin.h apngreader_p.h apngreader.cpp)

qt_add_plugin(ApngImagePlugin SHARED ${APNG_SOURCES})
target_link_libraries(ApngImagePlugin PRIVATE Qt6::Core Qt6::Gui png_static
                                              zlibstatic)
get_target_property(_png_include png_static INCLUDE_DIRECTORIES)
target_include_directories(ApngImagePlugin PRIVATE ${_png_include})

set_target_properties(
  ApngImagePlugin
  PROPERTIES AUTOMOC ON LIBRARY_OUTPUT_DIRECTORY
                        "${CMAKE_BINARY_DIR}/plugins/imageformats")
