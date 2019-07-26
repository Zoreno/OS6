#include <cxxabi.h>

namespace __cxxabiv1
{
__function_type_info::~__function_type_info() {}

bool __function_type_info::__is_function_p() const
{
	return true;
}

} // namespace __cxxabiv1