#include "ComputerTest.h"

#include <filesystem>

class CompiledProgramTest : public ComputerTest {};

TEST_F(CompiledProgramTest, Program1)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program1.out");
	// lda #$ff
	// sta $3002

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xFF);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x3002), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(CompiledProgramTest, Program2)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program2.out");
	// ldx #$05
	// lda #$42
	// sta $20, X
	// ldy #$ff
	// ldx $26, Y
	// stx $1166

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.Y, 0xFF);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x25), 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x1166), 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 18);
}

TEST_F(CompiledProgramTest, Program3)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program3.out");
	// lda #$42
	// sta $62
	// lda #$33
	// clc
	// adc $62
	// sta $3320

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x75);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x62), 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x3320), 0x75);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 16);
}

