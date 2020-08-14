#==============================================================================
# This file sets the cmake compiler configuration to be able to link the 
# library under test
#==============================================================================

set(CMAKE_CXX_COMPILER /usr/bin/g++-9)
set(CMAKE_CXX_COMPILER_FLAGS "-std=c++11 -nostdlib -lgcc -fno-rtti -fno-exceptions /usr/lib/x86_64-linux-gnu/crt* -lc")

#==============================================================================
# End of file
#==============================================================================
