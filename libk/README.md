# Kernel standard C library

This is the c standard library for the kernel, which is statically linked to the kernel executable. It contains a subset of the standard library functions. 

## Missing functionality

* Locale handling
* Reentrant versions of functions
* Math functions for double and long double
* Proper file handling

## Math functions

Math functions are put in the same archive. Most math functions are implemented as simple as possible, so the precision of the functions may wary.