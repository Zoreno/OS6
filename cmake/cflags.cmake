#==============================================================================
# Flags and compiler configuration
#==============================================================================

# Debug build
set(DEBUG_BUILD true)

# Should build the unit tests?
set(BUILDING_TESTS false)

set(CFLAGS "-m64")

# Set warnings and error conditions
set(CFLAGS "${CFLAGS} -Wall -Wextra -Werror=implicit-function-declaration -Wno-unused -Wno-sign-compare")

# Disable standard library
set(CFLAGS "${CFLAGS} -ffreestanding")

# Disable language features
set(CFLAGS "${CFLAGS} -fno-builtin -finline-functions -fno-stack-protector -mno-red-zone")

# Cpp flags
set(CPPFLAGS "${CFLAGS} -std=c++11 -static-libgcc")

# Set C standard
set(CFLAGS "${CFLAGS} -Wno-pointer-sign -std=c11")

set(ASMFLAGS "")

if(${DEBUG_BUILD})

    # Setup dwarf information
    set(CFLAGS "${CFLAGS} -DOS6_DEBUG -gdwarf-4")
    set(ASMFLAGS "${ASMFLAGS} -g -F dwarf")
    #set(CMAKE_CXX_CPPCHECK "cppcheck")
    #set(CMAKE_C_CPPCHECK "cppcheck")

else()

    # Set full optimization 
    set(CFLAGS "${CFLAGS} -DOS6_RELEASE -O3")
    set(ASMFLAGS "${ASMFLAGS} -Ox")

endif()