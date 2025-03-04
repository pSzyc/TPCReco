set(MODULE_NAME "Reconstruction")
message(STATUS "Adding CMake fragment for module:\t${MODULE_NAME}")

reco_add_library(${MODULE_NAME})

set(TENSORFLOW_INCLUDE_DIR /usr/local/include)
set(TENSORFLOW_LIB_DIR /usr/local/lib)
link_directories(${TENSORFLOW_LIB_DIR})

target_include_directories(${MODULE_NAME} PRIVATE ${TENSORFLOW_INCLUDE_DIR})

target_link_libraries(
  ${MODULE_NAME}
  PUBLIC ${ROOT_LIBRARIES} ${ROOT_EXE_LINKER_FLAGS} DataFormats Utilities
  PRIVATE Resources tensorflow) # Link TensorFlow library

install(DIRECTORY examples DESTINATION ${CMAKE_INSTALL_PREFIX})

reco_add_executable(runTensorflow bin/runTensorflow.cpp)
target_link_libraries(runTensorflow PRIVATE Boost::program_options ${MODULE_NAME} MonteCarloModules UtilsMC)
reco_install_targets(${MODULE_NAME} runTensorflow)

install(DIRECTORY config DESTINATION ${CMAKE_INSTALL_PREFIX})
