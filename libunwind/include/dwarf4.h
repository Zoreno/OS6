/**
 * @file dwarf4.h
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

#ifndef _LIBUNWIND_DWARF4_H
#define _LIBUNWIND_DWARF4_H

#include <eh_frame.h>
#include <registers_intel_x64.h>
#include <stdint.h>

#define MAX_ROWS 17

enum register_rules
{
    rule_undefined = 0,
    rule_same_value = 1,
    rule_offsetn = 2,
    rule_val_offsetn = 3,
    rule_register = 4,
    rule_expression = 5,
    rule_val_expression = 6
};

#define DW_CFA_advance_loc 0x40
#define DW_CFA_offset 0x80
#define DW_CFA_restore 0xC0
#define DW_CFA_nop 0x00
#define DW_CFA_set_loc 0x01
#define DW_CFA_advance_loc1 0x02
#define DW_CFA_advance_loc2 0x03
#define DW_CFA_advance_loc4 0x04
#define DW_CFA_offset_extended 0x05
#define DW_CFA_restore_extended 0x06
#define DW_CFA_undefined 0x07
#define DW_CFA_same_value 0x08
#define DW_CFA_register 0x09
#define DW_CFA_remember_state 0x0A
#define DW_CFA_restore_state 0x0B
#define DW_CFA_def_cfa 0x0C
#define DW_CFA_def_cfa_register 0x0D
#define DW_CFA_def_cfa_offset 0x0E
#define DW_CFA_def_cfa_expression 0x0F
#define DW_CFA_expression 0x10
#define DW_CFA_offset_extended_sf 0x11
#define DW_CFA_def_cfa_sf 0x12
#define DW_CFA_def_cfa_offset_sf 0x13
#define DW_CFA_val_offset 0x14
#define DW_CFA_val_offset_sf 0x15
#define DW_CFA_val_expression 0x16

#define DW_OP_addr 0x03
#define DW_OP_deref 0x06
#define DW_OP_const1u 0x08
#define DW_OP_const1s 0x09
#define DW_OP_const2u 0x0A
#define DW_OP_const2s 0x0B
#define DW_OP_const4u 0x0C
#define DW_OP_const4s 0x0D
#define DW_OP_const8u 0x0E
#define DW_OP_const8s 0x0F
#define DW_OP_constu 0x10
#define DW_OP_consts 0x11
#define DW_OP_dup 0x12
#define DW_OP_drop 0x13
#define DW_OP_over 0x14
#define DW_OP_pick 0x15
#define DW_OP_swap 0x16
#define DW_OP_rot 0x17
#define DW_OP_xderef 0x18
#define DW_OP_abs 0x19
#define DW_OP_and 0x1A
#define DW_OP_div 0x1B
#define DW_OP_minus 0x1C
#define DW_OP_mod 0x1D
#define DW_OP_mul 0x1E
#define DW_OP_neg 0x1F
#define DW_OP_not 0x20
#define DW_OP_or 0x21
#define DW_OP_plus 0x22
#define DW_OP_plus_uconst 0x23
#define DW_OP_shl 0x24
#define DW_OP_shr 0x25
#define DW_OP_shra 0x26
#define DW_OP_xor 0x27
#define DW_OP_skip 0x2F
#define DW_OP_bra 0x28
#define DW_OP_eq 0x29
#define DW_OP_ge 0x2A
#define DW_OP_gt 0x2B
#define DW_OP_le 0x2C
#define DW_OP_lt 0x2D
#define DW_OP_ne 0x2E
#define DW_OP_lit0 0x30
#define DW_OP_lit1 0x31
#define DW_OP_lit2 0x32
#define DW_OP_lit3 0x33
#define DW_OP_lit4 0x34
#define DW_OP_lit5 0x35
#define DW_OP_lit6 0x36
#define DW_OP_lit7 0x37
#define DW_OP_lit8 0x38
#define DW_OP_lit9 0x39
#define DW_OP_lit10 0x3A
#define DW_OP_lit11 0x3B
#define DW_OP_lit12 0x3C
#define DW_OP_lit13 0x3D
#define DW_OP_lit14 0x3E
#define DW_OP_lit15 0x3F
#define DW_OP_lit16 0x40
#define DW_OP_lit17 0x41
#define DW_OP_lit18 0x42
#define DW_OP_lit19 0x43
#define DW_OP_lit20 0x44
#define DW_OP_lit21 0x45
#define DW_OP_lit22 0x46
#define DW_OP_lit23 0x47
#define DW_OP_lit24 0x48
#define DW_OP_lit25 0x49
#define DW_OP_lit26 0x4A
#define DW_OP_lit27 0x4B
#define DW_OP_lit28 0x4C
#define DW_OP_lit29 0x4D
#define DW_OP_lit30 0x4E
#define DW_OP_lit31 0x4F
#define DW_OP_reg0 0x50
#define DW_OP_reg1 0x51
#define DW_OP_reg2 0x52
#define DW_OP_reg3 0x53
#define DW_OP_reg4 0x54
#define DW_OP_reg5 0x55
#define DW_OP_reg6 0x56
#define DW_OP_reg7 0x57
#define DW_OP_reg8 0x58
#define DW_OP_reg9 0x59
#define DW_OP_reg10 0x5A
#define DW_OP_reg11 0x5B
#define DW_OP_reg12 0x5C
#define DW_OP_reg13 0x5D
#define DW_OP_reg14 0x5E
#define DW_OP_reg15 0x5F
#define DW_OP_reg16 0x60
#define DW_OP_reg17 0x61
#define DW_OP_reg18 0x62
#define DW_OP_reg19 0x63
#define DW_OP_reg20 0x64
#define DW_OP_reg21 0x65
#define DW_OP_reg22 0x66
#define DW_OP_reg23 0x67
#define DW_OP_reg24 0x68
#define DW_OP_reg25 0x69
#define DW_OP_reg26 0x6A
#define DW_OP_reg27 0x6B
#define DW_OP_reg28 0x6C
#define DW_OP_reg29 0x6D
#define DW_OP_reg30 0x6E
#define DW_OP_reg31 0x6F
#define DW_OP_breg0 0x70
#define DW_OP_breg1 0x71
#define DW_OP_breg2 0x72
#define DW_OP_breg3 0x73
#define DW_OP_breg4 0x74
#define DW_OP_breg5 0x75
#define DW_OP_breg6 0x76
#define DW_OP_breg7 0x77
#define DW_OP_breg8 0x78
#define DW_OP_breg9 0x79
#define DW_OP_breg10 0x7A
#define DW_OP_breg11 0x7B
#define DW_OP_breg12 0x7C
#define DW_OP_breg13 0x7D
#define DW_OP_breg14 0x7E
#define DW_OP_breg15 0x7F
#define DW_OP_breg16 0x80
#define DW_OP_breg17 0x81
#define DW_OP_breg18 0x82
#define DW_OP_breg19 0x83
#define DW_OP_breg20 0x84
#define DW_OP_breg21 0x85
#define DW_OP_breg22 0x86
#define DW_OP_breg23 0x87
#define DW_OP_breg24 0x88
#define DW_OP_breg25 0x89
#define DW_OP_breg26 0x8A
#define DW_OP_breg27 0x8B
#define DW_OP_breg28 0x8C
#define DW_OP_breg29 0x8D
#define DW_OP_breg30 0x8E
#define DW_OP_breg31 0x8F
#define DW_OP_regx 0x90
#define DW_OP_fbreg 0x91
#define DW_OP_bregx 0x92
#define DW_OP_piece 0x93
#define DW_OP_deref_size 0x94
#define DW_OP_xderef_size 0x95
#define DW_OP_nop 0x96
#define DW_OP_push_object_addres 0x97
#define DW_OP_call2 0x98
#define DW_OP_call4 0x99
#define DW_OP_call_ref 0x9A
#define DW_OP_form_tls_address 0x9B
#define DW_OP_call_frame_cfa 0x9C
#define DW_OP_bit_piece 0x9D
#define DW_OP_implicit_value 0x9E
#define DW_OP_stack_value 0x9F
#define DW_OP_lo_user 0xE0
#define DW_OP_hi_user 0xFF

class dwarf4
{
public:
    static int64_t decode_sleb128(char **addr);
    static uint64_t decode_uleb128(char **addr);
    static void unwind(const fd_entry &fde, register_state *state = nullptr);
};

#endif

//=============================================================================
// End of file
//=============================================================================