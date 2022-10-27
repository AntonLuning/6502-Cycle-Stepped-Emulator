#pragma once

#include "Base.h"

// ADDRESS MODES
// A		: Accumulator
// abs		: Absolute
// abs,X	: Absolute, X-indexed
// abs,Y	: Absolute, Y-indexed
// #		: Immediate
// impl		: Implied
// ind		: Indirect
// X,ind	: X-indexed, indirect
// ind,Y	: Indirect, Y-indexed
// rel		: Relative
// zpg		: Zeropage
// zpg,X	: Zeropage, X-indexed
// zpg,Y	: Zeropage, Y-indexed

// -------------------------------------------------------------
// ------------------- TRANSFER INSTRUCTIONS -------------------
// -------------------------------------------------------------

// LDA - Load accumulator
#define INS_LDA_IM		((BYTE)0xA9)	// Immediate
#define INS_LDA_ZP		((BYTE)0xA5)	// Zero Page
#define INS_LDA_ZPX		((BYTE)0xB5)	// Zero Page X
#define INS_LDA_ABS		((BYTE)0xAD)	// Absolute
#define INS_LDA_ABSX	((BYTE)0xBD)	// Absolute X
#define INS_LDA_ABSY	((BYTE)0xB9)	// Absolute Y
#define INS_LDA_INDX	((BYTE)0xA1)	// Indirect X
#define INS_LDA_INDY	((BYTE)0xB1)	// Indirect Y

// LDX - Load X
#define INS_LDX_IM		((BYTE)0xA2)	// Immediate
#define INS_LDX_ZP		((BYTE)0xA6)	// Zero Page
#define INS_LDX_ZPY		((BYTE)0xB6)	// Zero Page Y
#define INS_LDX_ABS		((BYTE)0xAE)	// Absolute
#define INS_LDX_ABSY	((BYTE)0xBE)	// Absolute Y

// LDY - Load Y
#define INS_LDY_IM		((BYTE)0xA0)	// Immediate
#define INS_LDY_ZP		((BYTE)0xA4)	// Zero Page
#define INS_LDY_ZPX		((BYTE)0xB4)	// Zero Page X
#define INS_LDY_ABS		((BYTE)0xAC)	// Absolute
#define INS_LDY_ABSX	((BYTE)0xBC)	// Absolute X

// STA - Store accumulator
#define INS_STA_ZP		((BYTE)0x85)	// Zero Page
#define INS_STA_ZPX		((BYTE)0x95)	// Zero Page X
#define INS_STA_ABS		((BYTE)0x8D)	// Absolute
#define INS_STA_ABSX	((BYTE)0x9D)	// Absolute X
#define INS_STA_ABSY	((BYTE)0x99)	// Absolute Y
#define INS_STA_INDX	((BYTE)0x81)	// Indirect X
#define INS_STA_INDY	((BYTE)0x91)	// Indirect Y

// STX - Store X
#define INS_STX_ZP		((BYTE)0x86)	// Zero Page
#define INS_STX_ZPY		((BYTE)0x96)	// Zero Page Y
#define INS_STX_ABS		((BYTE)0x8E)	// Absolute

// STY - Store Y
#define INS_STY_ZP		((BYTE)0x84)	// Zero Page
#define INS_STY_ZPX		((BYTE)0x94)	// Zero Page X
#define INS_STY_ABS		((BYTE)0x8C)	// Absolute

// TAX - Transfer accumulator to X
#define INS_TAX_IMP		((BYTE)0xAA)	// Implied

// TAY - Transfer accumulator to Y
#define INS_TAY_IMP		((BYTE)0xA8)	// Implied

// TSX - Transfer stack pointer to X
#define INS_TSX_IMP		((BYTE)0xBA)	// Implied

// TXA - Transfer X to accumulator
#define INS_TXA_IMP		((BYTE)0x8A)	// Implied

// TXS - Transfer X to stack pointer
#define INS_TXS_IMP		((BYTE)0x9A)	// Implied

// TYA - Transfer Y to accumulator
#define INS_TYA_IMP		((BYTE)0x98)	// Implied


// -------------------------------------------------------------
// ----------------- PUSH & PULL INSTRUCTIONS ------------------
// -------------------------------------------------------------

// PHA - Push accumulator
#define INS_PHA_IMP		((BYTE)0x48)	// Implied
// PLA - Pull accumulator
#define INS_PLA_IMP		((BYTE)0x68)	// Implied

// PHP - Push processor status (PS)
#define INS_PHP_IMP		((BYTE)0x08)	// Implied
// PLP - Pull processor status (PS)
#define INS_PLP_IMP		((BYTE)0x28)	// Implied


// -------------------------------------------------------------
// ----------------- DECREMENTS & INCREMENTS -------------------
// -------------------------------------------------------------

// DEC - Decrement
#define INS_DEC_ZP		((BYTE)0xC6)	// Zero Page
#define INS_DEC_ZPX		((BYTE)0xD6)	// Zero Page X
#define INS_DEC_ABS		((BYTE)0xCE)	// Absolute
#define INS_DEC_ABSX	((BYTE)0xDE)	// Absolute X

// DEX - Decrement X
#define INS_DEX_IMP		((BYTE)0xCA)	// Implied

// DEY - Decrement Y
#define INS_DEY_IMP		((BYTE)0x88)	// Implied

// INC - Increment
#define INS_INC_ZP		((BYTE)0xE6)	// Zero Page
#define INS_INC_ZPX		((BYTE)0xF6)	// Zero Page X
#define INS_INC_ABS		((BYTE)0xEE)	// Absolute
#define INS_INC_ABSX	((BYTE)0xFE)	// Absolute X

// INX - Increment X
#define INS_INX_IMP		((BYTE)0xE8)	// Implied

// INY - Increment Y
#define INS_INY_IMP		((BYTE)0xC8)	// Implied


// -------------------------------------------------------------
// ----------------------- ARITHMETIC --------------------------
// -------------------------------------------------------------

// ADC - Add with carry
#define INS_ADC_IM		((BYTE)0x69)	// Immediate
#define INS_ADC_ZP		((BYTE)0x65)	// Zero Page
#define INS_ADC_ZPX		((BYTE)0x75)	// Zero Page X
#define INS_ADC_ABS		((BYTE)0x6D)	// Absolute
#define INS_ADC_ABSX	((BYTE)0x7D)	// Absolute X
#define INS_ADC_ABSY	((BYTE)0x79)	// Absolute Y
#define INS_ADC_INDX	((BYTE)0x61)	// Indirect X
#define INS_ADC_INDY	((BYTE)0x71)	// Indirect Y

// SBC - Subtract with carry
#define INS_SBC_IM		((BYTE)0xE9)	// Immediate
#define INS_SBC_ZP		((BYTE)0xE5)	// Zero Page
#define INS_SBC_ZPX		((BYTE)0xF5)	// Zero Page X
#define INS_SBC_ABS		((BYTE)0xED)	// Absolute
#define INS_SBC_ABSX	((BYTE)0xFD)	// Absolute X
#define INS_SBC_ABSY	((BYTE)0xF9)	// Absolute Y
#define INS_SBC_INDX	((BYTE)0xE1)	// Indirect X
#define INS_SBC_INDY	((BYTE)0xF1)	// Indirect Y


// -------------------------------------------------------------
// ----------------- LOGICAL, SHIFT & ROTATE -------------------
// -------------------------------------------------------------

// AND - And (with accumulator)
#define INS_AND_IM		((BYTE)0x29)	// Immediate
#define INS_AND_ZP		((BYTE)0x25)	// Zero Page
#define INS_AND_ZPX		((BYTE)0x35)	// Zero Page X
#define INS_AND_ABS		((BYTE)0x2D)	// Absolute
#define INS_AND_ABSX	((BYTE)0x3D)	// Absolute X
#define INS_AND_ABSY	((BYTE)0x39)	// Absolute Y
#define INS_AND_INDX	((BYTE)0x21)	// Indirect X
#define INS_AND_INDY	((BYTE)0x31)	// Indirect Y

// ORA - Or (with accumulator)
#define INS_ORA_IM		((BYTE)0x09)	// Immediate
#define INS_ORA_ZP		((BYTE)0x05)	// Zero Page
#define INS_ORA_ZPX		((BYTE)0x15)	// Zero Page X
#define INS_ORA_ABS		((BYTE)0x0D)	// Absolute
#define INS_ORA_ABSX	((BYTE)0x1D)	// Absolute X
#define INS_ORA_ABSY	((BYTE)0x19)	// Absolute Y
#define INS_ORA_INDX	((BYTE)0x01)	// Indirect X
#define INS_ORA_INDY	((BYTE)0x11)	// Indirect Y

// EOR - Exclusive or (with accumulator)
#define INS_EOR_IM		((BYTE)0x49)	// Immediate
#define INS_EOR_ZP		((BYTE)0x45)	// Zero Page
#define INS_EOR_ZPX		((BYTE)0x55)	// Zero Page X
#define INS_EOR_ABS		((BYTE)0x4D)	// Absolute
#define INS_EOR_ABSX	((BYTE)0x5D)	// Absolute X
#define INS_EOR_ABSY	((BYTE)0x59)	// Absolute Y
#define INS_EOR_INDX	((BYTE)0x41)	// Indirect X
#define INS_EOR_INDY	((BYTE)0x51)	// Indirect Y

// ASL - Arithmetic shift left
#define INS_ASL_ACC		((BYTE)0x0A)	// Accumulator
#define INS_ASL_ZP		((BYTE)0x06)	// Zero Page
#define INS_ASL_ZPX		((BYTE)0x16)	// Zero Page X
#define INS_ASL_ABS		((BYTE)0x0E)	// Absolute
#define INS_ASL_ABSX	((BYTE)0x1E)	// Absolute X

// LSR - Logical shift right
#define INS_LSR_ACC		((BYTE)0x4A)	// Accumulator
#define INS_LSR_ZP		((BYTE)0x46)	// Zero Page
#define INS_LSR_ZPX		((BYTE)0x56)	// Zero Page X
#define INS_LSR_ABS		((BYTE)0x4E)	// Absolute
#define INS_LSR_ABSX	((BYTE)0x5E)	// Absolute X

// ROL - Rotate left
#define INS_ROL_ACC		((BYTE)0x2A)	// Accumulator
#define INS_ROL_ZP		((BYTE)0x26)	// Zero Page
#define INS_ROL_ZPX		((BYTE)0x36)	// Zero Page X
#define INS_ROL_ABS		((BYTE)0x2E)	// Absolute
#define INS_ROL_ABSX	((BYTE)0x3E)	// Absolute X

// ROR - Rotate right
#define INS_ROR_ACC		((BYTE)0x6A)	// Accumulator
#define INS_ROR_ZP		((BYTE)0x66)	// Zero Page
#define INS_ROR_ZPX		((BYTE)0x76)	// Zero Page X
#define INS_ROR_ABS		((BYTE)0x6E)	// Absolute
#define INS_ROR_ABSX	((BYTE)0x7E)	// Absolute X


// -------------------------------------------------------------
// -------------------- FLAG INSTRUCTIONS ----------------------
// -------------------------------------------------------------

// CLC - Clear carry
#define INS_CLC_IMP		((BYTE)0x18)	// Implied

// CLD - Clear decimal
#define INS_CLD_IMP		((BYTE)0xD8)	// Implied

// CLI - Clear interrupt disable
#define INS_CLI_IMP		((BYTE)0x58)	// Implied

// CLV - Clear overflow
#define INS_CLV_IMP		((BYTE)0xB8)	// Implied

// SEC - Set carry
#define INS_SEC_IMP		((BYTE)0x38)	// Implied

// SED - Set decimal
#define INS_SED_IMP		((BYTE)0xF8)	// Implied

// SEI - Set interrupt disable
#define INS_SEI_IMP		((BYTE)0x78)	// Implied


// -------------------------------------------------------------
// ------------------- BRANCH & COMPARISONS --------------------
// -------------------------------------------------------------

// BCC - Branch on carry clear
#define INS_BCC_REL		((BYTE)0x90)	// Relative
// BCS - Branch on carry set
#define INS_BCS_REL		((BYTE)0xB0)	// Relative

// BNE - Branch on not equal (zero clear)
#define INS_BNE_REL		((BYTE)0xD0)	// Relative
// BEQ - Branch on equal (zero set)
#define INS_BEQ_REL		((BYTE)0xF0)	// Relative

// BPL - Branch on plus (negative clear)
#define INS_BPL_REL		((BYTE)0x10)	// Relative
// BMI - Branch on minus (negative set)
#define INS_BMI_REL		((BYTE)0x30)	// Relative

// BVC - Branch on overflow clear
#define INS_BVC_REL		((BYTE)0x50)	// Relative
// BVS - Branch on overflow set
#define INS_BVS_REL		((BYTE)0x70)	// Relative

// CMP - Compare (with accumulator)
#define INS_CMP_IM		((BYTE)0xC9)	// Immediate
#define INS_CMP_ZP		((BYTE)0xC5)	// Zero Page
#define INS_CMP_ZPX		((BYTE)0xD5)	// Zero Page X
#define INS_CMP_ABS		((BYTE)0xCD)	// Absolute
#define INS_CMP_ABSX	((BYTE)0xDD)	// Absolute X
#define INS_CMP_ABSY	((BYTE)0xD9)	// Absolute Y
#define INS_CMP_INDX	((BYTE)0xC1)	// Indirect X
#define INS_CMP_INDY	((BYTE)0xD1)	// Indirect Y

// CPX - Compare with X
#define INS_CPX_IM		((BYTE)0xE0)	// Immediate
#define INS_CPX_ZP		((BYTE)0xE4)	// Zero Page
#define INS_CPX_ABS		((BYTE)0xEC)	// Absolute

// CPY - Compare with Y
#define INS_CPY_IM		((BYTE)0xC0)	// Immediate
#define INS_CPY_ZP		((BYTE)0xC4)	// Zero Page
#define INS_CPY_ABS		((BYTE)0xCC)	// Absolute


// -------------------------------------------------------------
// ------------- JUMP, SUBROUTINES & INTERRUPTS ----------------
// -------------------------------------------------------------

// BRK - Break / Interrupt
#define INS_BRK_IMP		((BYTE)0x00)	// Implied

// RTI - Return from interrupt
#define INS_RTI_IMP		((BYTE)0x40)	// Implied

// JMP - Jump
#define INS_JMP_ABS		((BYTE)0x4C)	// Absolute
#define INS_JMP_IND		((BYTE)0x6C)	// Indirect

// JSR - Jump subroutine
#define INS_JSR_ABS 	((BYTE)0x20)	// Absolute

// RTS - Return from subroutine
#define INS_RTS_IMP		((BYTE)0x60)	// Implied


// -------------------------------------------------------------
// ------------------------- OTHER -----------------------------
// -------------------------------------------------------------

// NOP - No operation
#define INS_NOP_IMP		((BYTE)0xEA)	// Implied

// BIT - Bit test
#define INS_BIT_ZP		((BYTE)0x24)	// Zero Page
#define INS_BIT_ABS		((BYTE)0x2C)	// Absolute
