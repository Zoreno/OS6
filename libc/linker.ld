ENTRY(_start)

SECTIONS
{
    . = 0x1000000;

    .text BLOCK(4K) : ALIGN(4K)
	{
		code = .;
		*(.text)
		*(.text*)
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
		*(.rodata*)
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

	.eh_frame_hdr : { *(.eh_frame_hdr) }
	__start_eh_frame = .;
	.eh_frame : ONLY_IF_RO { KEEP (*(.eh_frame)) }
	__end_eh_frame = .;
	.gcc_except_table : ONLY_IF_RO { *(.gcc_except_table) }

	.init :
	{
		KEEP (*(SORT_NONE(.init)))
	}

	.fini :
	{
		KEEP (*(SORT_NONE(.fini)))
	}

	.preinit_array :
	{
		__preinit_array_start = .;
		*(.preinit_array)
		__preinit_array_end = .;
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