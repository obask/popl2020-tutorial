include_directories(PRIVATE include)

add_executable(dse
  src/DSE.cpp
  src/Strategy.cpp
  )

add_llvm_library(InstrumentPass MODULE
  src/Instrument.cpp
  )

target_link_libraries(dse ${llvm_libs} ${Z3_LIBRARIES})

add_library(runtime MODULE
  src/SymbolicInterpreter.cpp
  src/Runtime.cpp
  )

target_link_libraries(runtime ${llvm_libs} ${Z3_LIBRARIES})
