#include <type_info.h>

namespace std
{
type_info::~type_info() {}

bool type_info::__is_pointer_p() const
{
	return false;
}

bool type_info::__is_function_p() const
{
	return false;
}

bool type_info::__do_catch(const type_info *thr_type, void **, unsigned) const
{
	return *this == *thr_type;
}

bool type_info::__do_upcast(const abi::__class_type_info *, void **) const
{
	return false;
}

} // namespace std