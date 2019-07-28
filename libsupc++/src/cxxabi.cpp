#include <cxxabi.h>

#include <stdio.h>

#define EXCEPTION_BUFF_SIZE 255
char exception_buff[EXCEPTION_BUFF_SIZE];

typedef const uint8_t *LSDA_ptr;

extern "C"
{
    int readSLEB128(const uint8_t *data)
    {
        uintptr_t result = 0;
        uintptr_t shift = 0;
        unsigned char byte;
        const uint8_t *p = data;
        do
        {
            byte = *p++;
            result |= static_cast<uintptr_t>(byte & 0x7F) << shift;
            shift += 7;
        } while (byte & 0x80);

        if ((byte & 0x40) && (shift < (sizeof(result) << 3)))
        {
            result |= static_cast<uintptr_t>(~0) << shift;
        }

        return static_cast<int>(result);
    }

    struct LSDA_Header
    {
        LSDA_Header(LSDA_ptr *lsda)
        {
            LSDA_ptr read_ptr = *lsda;

            start_encoding = read_ptr[0];
            type_encoding = read_ptr[1];
            type_table_offset = read_ptr[2];

            *lsda = read_ptr + sizeof(LSDA_Header);
        }

        uint8_t start_encoding;
        uint8_t type_encoding;
        uint8_t type_table_offset;
    };

    struct LSDA_CS_Header
    {
        LSDA_CS_Header(LSDA_ptr *lsda)
        {
            LSDA_ptr read_ptr = *lsda;
            encoding = read_ptr[0];
            length = read_ptr[1];
            *lsda = read_ptr + sizeof(LSDA_CS_Header);
        }

        uint8_t encoding;
        uint8_t length;
    };

    struct LSDA_CS
    {
        LSDA_CS(LSDA_ptr *lsda)
        {
            LSDA_ptr read_ptr = *lsda;
            start = read_ptr[0];
            len = read_ptr[1];
            lp = read_ptr[2];
            action = read_ptr[3];
            *lsda = read_ptr + sizeof(LSDA_CS);
        }

        LSDA_CS() {}

        uint8_t start;
        uint8_t len;
        uint8_t lp;
        uint8_t action;

        bool has_landing_pad() const
        {
            return lp;
        }

        bool valid_for_throw_ip(uintptr_t func_start, uintptr_t throw_ip) const
        {
            uintptr_t try_start = func_start + this->start;
            uintptr_t try_end = func_start + this->start + this->len;

            if (throw_ip < try_start)
            {
                return false;
            }

            if (throw_ip > try_end)
            {
                return false;
            }

            return true;
        }
    };

    struct LSDA
    {
        LSDA_Header header;
        const void **types_table_start;
        LSDA_CS_Header cs_header;
        const LSDA_ptr cs_table_start;
        const LSDA_ptr cs_table_end;
        const LSDA_ptr action_table_start;

        LSDA(LSDA_ptr raw_lsda)
            : header(&raw_lsda),
              types_table_start((const void **)(raw_lsda + header.type_table_offset)),
              cs_header(&raw_lsda),
              cs_table_start(raw_lsda),
              cs_table_end(raw_lsda + cs_header.length),
              action_table_start(cs_table_end)
        {
        }

        LSDA_CS next_cs_entry;
        LSDA_ptr next_cs_entry_ptr;

        const LSDA_CS *next_call_site_entry(bool start = false)
        {
            if (start)
            {
                next_cs_entry_ptr = cs_table_start;
            }

            if (next_cs_entry_ptr >= cs_table_end)
            {
                return (const LSDA_CS *)0;
            }

            next_cs_entry = LSDA_CS(&next_cs_entry_ptr);

            return &next_cs_entry;
        }

        LSDA_ptr get_action_for_call_site(const LSDA_CS *cs) const
        {
            if (cs->action == 0)
            {
                return (LSDA_ptr)0;
            }

            const size_t action_offset = cs->action - 1;
            return this->action_table_start + action_offset;
        }

        struct Action
        {
            int type_index;

            int next_offset;

            LSDA_ptr raw_action_ptr;
        } current_action;

        const Action *get_first_action_for_cs(const LSDA_CS *cs)
        {
            if (cs->action == 0)
            {
                return 0;
            }

            const size_t action_offset = cs->action - 1;
            LSDA_ptr action_raw = this->action_table_start + action_offset;

            current_action.type_index = action_raw[0];
            current_action.next_offset = readSLEB128(&action_raw[1]);
            current_action.raw_action_ptr = &action_raw[0];

            return &current_action;
        }

        const Action *get_next_action()
        {
            if (current_action.next_offset == 0)
            {
                return 0;
            }

            LSDA_ptr action_raw = current_action.raw_action_ptr + 1 + current_action.next_offset;

            current_action.type_index = action_raw[0];
            current_action.next_offset = readSLEB128(&action_raw[1]);
            current_action.raw_action_ptr = &action_raw[0];

            return &current_action;
        }

        const std::type_info *get_type_for(const Action *action) const
        {
            int idx = -1 * action->type_index;

            const void *catch_type_info = this->types_table_start[idx];

            return (const std::type_info *)catch_type_info;
        }
    };

    bool can_handle(const std::type_info *thrown_exception, const std::type_info *catch_type)
    {
        if (!catch_type)
        {
            return true;
        }

        if (thrown_exception == catch_type)
        {
            return true;
        }

        return false;
    }

    _Unwind_Reason_Code
    run_landing_pad(
        _Unwind_Exception *unwind_exception,
        _Unwind_Context *context,
        int exception_type_idx,
        uintptr_t lp_address)
    {
        int r0 = __builtin_eh_return_data_regno(0);
        int r1 = __builtin_eh_return_data_regno(1);

        _Unwind_SetGR(context, r0, (uintptr_t)(unwind_exception));
        _Unwind_SetGR(context, r1, (uintptr_t)(exception_type_idx));
        _Unwind_SetIP(context, lp_address);

        return _URC_INSTALL_CONTEXT;
    }

    void *__cxa_allocate_exception(size_t thrown_size)
    {
        puts("Allocating exception\n");

        if (thrown_size > EXCEPTION_BUFF_SIZE)
        {
            puts("Exception too big\n");
        }

        return &exception_buff;
    }

    void __cxa_free_exception(void *thrown_exception)
    {
        puts("Freeing exception\n");
    }

    extern void _exit(int ret);

#define _CASE(x)    \
    case x:         \
        puts(#x);   \
        puts("\n"); \
        break;

    void __cxa_throw(void *thrown_exception, std::type_info *tinfo, void (*dest)(void *))
    {
        puts("Throw!\n");

        __cxa_exception *header = ((__cxa_exception *)thrown_exception - 1);

        header->exception_type = tinfo;

        _Unwind_Reason_Code ret = _Unwind_RaiseException(&header->unwind_header);

        switch (ret)
        {
            _CASE(_URC_NO_REASON)
            _CASE(_URC_FOREIGN_EXCEPTION_CAUGHT)
            _CASE(_URC_FATAL_PHASE2_ERROR)
            _CASE(_URC_FATAL_PHASE1_ERROR)
            _CASE(_URC_NORMAL_STOP)
            _CASE(_URC_END_OF_STACK)
            _CASE(_URC_HANDLER_FOUND)
            _CASE(_URC_INSTALL_CONTEXT)
            _CASE(_URC_CONTINUE_UNWIND)
        }

        __cxa_begin_catch(&header->unwind_header);

        puts("No one handled exception...\n");

        _exit(-145);
    }

    void *__cxa_begin_catch(_Unwind_Exception *exception)
    {
        puts("Begin catch\n");
    }

    void __cxa_end_catch()
    {
        puts("End catch\n");
    }

    _Unwind_Reason_Code __gxx_personality_v0(
        int version,
        _Unwind_Action actions,
        uint64_t exception_class,
        _Unwind_Exception *unwind_exception,
        _Unwind_Context *context)
    {
        puts("Personality function\n");

        uintptr_t throw_ip = _Unwind_GetIP(context) - 1;

        uintptr_t func_start = _Unwind_GetRegionStart(context);

        __cxa_exception *exception_header = (__cxa_exception *)(unwind_exception + 1) - 1;
        std::type_info *thrown_exception_type = exception_header->exception_type;

        LSDA_ptr raw_lsda = (LSDA_ptr)_Unwind_GetLanguageSpecificData(context);

        LSDA lsda(raw_lsda);

        for (const LSDA_CS *cs = lsda.next_call_site_entry(true); cs != NULL; cs = lsda.next_call_site_entry())
        {
            if (!cs->has_landing_pad())
            {
                continue;
            }

            if (!cs->valid_for_throw_ip(func_start, throw_ip))
            {
                continue;
            }

            for (const LSDA::Action *action = lsda.get_first_action_for_cs(cs);
                 action != NULL;
                 action = lsda.get_next_action())
            {
                if (action->type_index == 0)
                {
                    if (actions & _UA_CLEANUP_PHASE)
                    {
                        return run_landing_pad(unwind_exception, context, action->type_index, func_start + cs->lp);
                    }
                }
                else
                {
                    const std::type_info *catch_type = lsda.get_type_for(action);

                    if (can_handle(catch_type, thrown_exception_type))
                    {
                        if (actions & _UA_SEARCH_PHASE)
                        {
                            return _URC_HANDLER_FOUND;
                        }

                        return run_landing_pad(unwind_exception, context, action->type_index, func_start + cs->lp);
                    }
                }
            }
        }

        return _URC_CONTINUE_UNWIND;
    }
}