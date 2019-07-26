#ifndef _TYPE_INFO_H
#define _TYPE_INFO_H

#pragma GCC system_header

#include <cxxabi.h>

namespace __cxxabiv1
{
struct vtable_prefix
{
	ptrdiff_t whole_object;
	const __class_type_info *whole_type;
	const void *origin;
};

// TODO: Check why this is needed
extern "C++"
{
	template <typename T>
	inline const T *adjust_pointer(const void *base, ptrdiff_t offset)
	{
		return reinterpret_cast<const T *>(reinterpret_cast<const char *>(base) + offset);
	}
}

inline void const *convert_to_base(void const *addr, bool is_virtual, ptrdiff_t offset)
{
	if (is_virtual)
	{
		const void *vtable = *static_cast<const void *const *>(addr);

		offset = *adjust_pointer<ptrdiff_t>(vtable, offset);
	}

	return adjust_pointer<void>(addr, offset);
}

inline bool contained_p(__class_type_info::__sub_kind access_path)
{
	return access_path >= __class_type_info::__contained_mask;
}

inline bool public_p(__class_type_info::__sub_kind access_path)
{
	return access_path & __class_type_info::__contained_public_mask;
}

inline bool virtual_p(__class_type_info::__sub_kind access_path)
{
	return (access_path & __class_type_info::__contained_virtual_mask);
}

inline bool contained_public_p(__class_type_info::__sub_kind access_path)
{
	return ((access_path & __class_type_info::__contained_public) == __class_type_info::__contained_public);
}

inline bool contained_nonpublic_p(__class_type_info::__sub_kind access_path)
{
	return ((access_path & __class_type_info::__contained_public) == __class_type_info::__contained_mask);
}

inline bool contained_nonvirtual_p(__class_type_info::__sub_kind access_path)
{
	return ((access_path & (__class_type_info::__contained_mask | __class_type_info::__contained_virtual_mask)) == __class_type_info::__contained_mask);
}

static const __class_type_info *const nonvirtual_base_type =
	static_cast<const __class_type_info *>(0) + 1;

struct __class_type_info::__upcast_result
{
	const void *dst_ptr;
	__sub_kind part2dst;
	int src_details;
	const __class_type_info *base_type;

	__upcast_result(int d)
		: dst_ptr(NULL), part2dst(__unknown), src_details(d), base_type(reinterpret_cast<const __class_type_info *>(NULL)) {}
};

struct __class_type_info::__dyncast_result
{
	const void *dst_ptr;
	__sub_kind whole2dst;
	__sub_kind whole2src;
	__sub_kind dst2src;
	int whole_details;

	__dyncast_result(int details_ = __vmi_class_type_info::__flags_unknown_mask)
		: dst_ptr(NULL), whole2dst(__unknown), whole2src(__unknown), dst2src(__unknown), whole_details(details_) {}

protected:
	__dyncast_result(const __dyncast_result &);

	__dyncast_result &operator=(const __dyncast_result &);
};

inline __class_type_info::__sub_kind __class_type_info::__find_public_src(ptrdiff_t src2dst,
																		  const void *obj_ptr,
																		  const __class_type_info *src_type,
																		  const void *src_ptr) const
{
	if (src2dst >= 0)
	{
		return adjust_pointer<void>(obj_ptr, src2dst) == src_ptr ? __contained_public : __not_contained;
	}

	if (src2dst == -2)
	{
		return __not_contained;
	}

	return __do_find_public_src(src2dst, obj_ptr, src_type, src_ptr);
}

} // namespace __cxxabiv1

#endif