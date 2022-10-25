#include "ComputerTest.h"

#include <filesystem>

class CompiledProgramTest : public ComputerTest {};

TEST_F(CompiledProgramTest, Program1)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program1.out");
	//		.org $c000
	//	reset :
	//		lda #$ff
	//		sta $3002
	//
	//		nop
	//		nop
	//
	//		.org $fffc
	//		.word reset
	//		.word $0000

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xFF);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x3002), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(CompiledProgramTest, Program2)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program2.out");
	//		.org $c000
	//	reset :
	//		ldx #$05
	//		lda #$42
	//		sta $20, X
	//		ldy #$ff
	//		ldx $26, Y
	//		stx $1166
	//
	//		nop
	//		nop
	//
	//		.org $fffc
	//		.word reset
	//		.word $0000

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.Y, 0xFF);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x25), 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x1166), 0x42);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 18);
}

TEST_F(CompiledProgramTest, Program3)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program3.out");
	//		.org $c000
	//	reset :
	//		lda #$42
	//		sta $62
	//		lda #$33
	//		clc
	//		adc $62
	//		sta $3320
	//
	//		nop
	//		nop
	//
	//		.org $fffc
	//		.word reset
	//		.word $0000

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x75);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x62), 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x3320), 0x75);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 16);
}

TEST_F(CompiledProgramTest, Program4)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program4.out");
	//		.org $c000
	//	reset :
	//		lda #$90
	//		clc
	//		sbc #$60
	//		sta $3320
	//
	//		nop
	//		nop
	//
	//		.org $fffc
	//		.word reset
	//		.word $0000

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x30);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x3320), 0x30);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 10);
}

TEST_F(CompiledProgramTest, Program5)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program5.out");
	//		.org $c000
	//	reset :
	//		lda #$90
	//		clc
	//		adc #$60
	//		brk
	//		nop
	//		sta $3320
	//
	//		nop
	//		nop
	//
	//		.org $f000
	//	interrupt :
	//		pha
	//		lda #$42
	//		sta $1562
	//		pla
	//		rti
	//
	//		.org $fffa
	//		.word interrupt
	//		.word reset
	//		.word interrupt

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF0);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x3320), 0xF0);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x1562), 0x42);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
}