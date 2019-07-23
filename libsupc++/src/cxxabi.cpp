#include <cxxabi.h>

#include <stdio.h>

#define EXCEPTION_BUFF_SIZE 255
char exception_buff[EXCEPTION_BUFF_SIZE];

typedef const uint8_t *LSDA_ptr;

struct LSDA_Header
{
    LSDA_Header(LSDA_ptr *lsda)
    {
        LSDA_ptr read_ptr = *lsda;

        start_encoding = read_ptr[0];
        type_encoding = read_ptr[1];
        ttype = read_ptr[2];

        *lsda = read_ptr + sizeof(LSDA_Header);
    }

    uint8_t start_encoding;
    uint8_t type_encoding;
    uint8_t ttype;
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

    uint8_t start;
    uint8_t len;
    uint8_t lp;
    uint8_t action;
};

extern "C"
{
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

    void __cxa_throw(void *thrown_exception, struct type_info *tinfo, void (*dest)(void *))
    {
        puts("Throw!\n");

        __cxa_exception *header = ((__cxa_exception *)thrown_exception - 1);
        _Unwind_RaiseException(&header->unwind_header);

        puts("No one handled exception...\n");

        _exit(-145);
    }

    void __cxa_begin_catch()
    {
        puts("Begin catch");
    }

    void __cxa_end_catch()
    {
        puts("End catch");
    }

    _Unwind_Reason_Code __gxx_personality_v0(
        int version,
        _Unwind_Action actions,
        uint64_t exception_class,
        _Unwind_Exception *unwind_exception,
        _Unwind_Context *context)
    {
        if (actions & _UA_SEARCH_PHASE)
        {
            puts("Personality lookup\n");

            return _URC_HANDLER_FOUND;
        }
        else if (actions & _UA_CLEANUP_PHASE)
        {
            puts("Personality cleanup\n");

            uintptr_t throw_ip = _Unwind_GetIP(context) - 1;

            const uint8_t *lsda = reinterpret_cast<const uint8_t *>(_Unwind_GetLanguageSpecificData(context));

            LSDA_Header header(&lsda);

            LSDA_CS_Header cs_header(&lsda);

            const LSDA_ptr lsda_cs_table_end = lsda + cs_header.length;

            while (lsda < lsda_cs_table_end)
            {
                LSDA_CS cs(&lsda);

                if (!cs.lp)
                {
                    continue;
                }

                uintptr_t func_start = _Unwind_GetRegionStart(context);

                uintptr_t try_start = func_start + cs.start;
                uintptr_t try_end = func_start + cs.start + cs.len;

                if (throw_ip < try_start)
                {
                    continue;
                }

                if (throw_ip > try_end)
                {
                    continue;
                }

                int r0 = __builtin_eh_return_data_regno(0);
                int r1 = __builtin_eh_return_data_regno(1);

                _Unwind_SetGR(context, r0, (uintptr_t)(unwind_exception));
                _Unwind_SetGR(context, r1, (uintptr_t)(1));

                _Unwind_SetIP(context, func_start + cs.lp);
                break;
            }

            return _URC_INSTALL_CONTEXT;
        }
        else
        {
            puts("Personality error\n");
            return _URC_FATAL_PHASE1_ERROR;
        }
    }
}