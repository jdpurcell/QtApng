qt_add_executable(ApngTests tst_apngplugin.cpp tst_apngplugin.qrc)
target_link_libraries(ApngTests PRIVATE Qt6::Test Qt6::Core Qt6::Gui)
set_target_properties(
  ApngTests
  PROPERTIES AUTOMOC ON
             AUTORCC ON
             RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
# To actually run the tests, you need to run [win/mac/linux]deployqt
