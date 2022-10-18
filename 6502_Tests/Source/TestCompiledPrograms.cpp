#include "ComputerTest.h"

#include <filesystem>

class CompiledProgramTest : public ComputerTest {};

TEST_F(CompiledProgramTest, Program1)
{
	MCU->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program1.out");

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