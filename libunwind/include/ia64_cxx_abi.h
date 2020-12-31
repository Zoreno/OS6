/**
 * @file ia64_cxx_abi.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-31
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#ifndef _LIBUNWIND_IA64_CXX_ABI_H
#define _LIBUNWIND_IA64_CXX_ABI_H

#include <new>
#include <stdint.h>

namespace std
{
class type_info;
}

struct _Unwind_Exception;
struct _Unwind_Context;

typedef enum
{
    _URC_NO_REASON = 0,
    _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
    _URC_FATAL_PHASE2_ERROR = 2,
    _URC_FATAL_PHASE1_ERROR = 3,
    _URC_NORMAL_STOP = 4,
    _URC_END_OF_STACK = 5,
    _URC_HANDLER_FOUND = 6,
    _URC_INSTALL_CONTEXT = 7,
    _URC_CONTINUE_UNWIND = 8

} _Unwind_Reason_Code;

typedef void (*_Unwind_Exception_Cleanup_Fn)(
    _Unwind_Reason_Code reason,
    _Unwind_Exception *exc);

struct _Unwind_Exception
{
    uint64_t exception_class;
    _Unwind_Exception_Cleanup_Fn exception_cleanup;
    uint64_t private_1;
    uint64_t private_2;
};

typedef int _Unwind_Action;
constexpr const _Unwind_Action _UA_SEARCH_PHASE = 1;
constexpr const _Unwind_Action _UA_CLEANUP_PHASE = 2;
constexpr const _Unwind_Action _UA_HANDLER_FRAME = 4;
constexpr const _Unwind_Action _UA_FORCE_UNWIND = 8;

extern "C"
{

    _Unwind_Reason_Code _Unwind_RaiseException(_Unwind_Exception *exception_object);
    void _Unwind_Resume(_Unwind_Exception *exception_object);
    void _Unwind_DeleteException(_Unwind_Exception *exception_object);
    uintptr_t _Unwind_GetGR(_Unwind_Context *context, int index);
    void _Unwind_SetGR(_Unwind_Context *context, int index, uintptr_t value);
    uintptr_t _Unwind_GetIP(_Unwind_Context *context);
    void _Unwind_SetIP(_Unwind_Context *context, uintptr_t value);
    uintptr_t _Unwind_GetLanguageSpecificData(_Unwind_Context *context);
    uintptr_t _Unwind_GetRegionStart(_Unwind_Context *context);
    typedef _Unwind_Reason_Code (
        *__personality_routine)(int version,
                                _Unwind_Action actions, uint64_t exceptionClass,
                                _Unwind_Exception *exceptionObject,
                                _Unwind_Context *context);
    uintptr_t _Unwind_GetIPInfo(_Unwind_Context *context, int *ip_before_insn);
}

#endif

//=============================================================================
// End of file
//=============================================================================