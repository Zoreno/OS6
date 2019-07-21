ENTRY(_start)

SECTIONS
{
    . = 0x1000000;

    .text BLOCK(4K) : ALIGN(4K)
	{
		code = .;
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		__start_ctors = .;
		*(SORT(.ctors*))
		__end_ctors = .;

		__start_dtors = .;
		*(SORT(.dtors*))
		__end_dtors = .;

		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	.eh_frame BLOCK(4K) : ALIGN(4K)
	{
		*(.eh_frame)
	}

	.init :
	{
		KEEP (*(SORT_NONE(.init)))
	}

	.fini :
	{
		KEEP (*(SORT_NONE(.fini)))
	}

	.init_array :
	{
		__init_array_start = .;
		*(.init_array)
		__init_array_end = .;
	}

	.fini_array :
	{
		__fini_array_start = .;
		*(.fini_array)
		__fini_array_end = .;
	}

	end = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.note.gnu.build-id)
	}
}