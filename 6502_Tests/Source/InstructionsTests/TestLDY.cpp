#include "../ComputerTest.h"

class LDYTest : public ComputerTest {};

TEST_F(LDYTest, LDYImmediate)
{
	BYTE program[] = {
		0xA0, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDYTest, LDYImmediateWithNegativeValue)
{
	BYTE program[] = {
		0xA0, 0xC2
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0xC2);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDYTest, LDYImmediateWithZeroValue)
{
	BYTE program[] = {
		0xA0, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDYTest, LDYZeroPage)
{
	BYTE program[] = {
		0xA4, 0xAA
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0xF2);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0xF2);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 3);
}

TEST_F(LDYTest, LDYZeroPageX)
{
	BYTE program[] = {
		0xB4, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0x42);
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDYTest, LDYZeroPageXWithZeroPageCrossover)
{
	BYTE program[] = {
		0xB4, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xA7, 0x42);
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDYTest, LDYAbsolute)
{
	BYTE program[] = {
		0xAC, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F33, 0x42);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDYTest, LDYAbsoluteX)
{
	BYTE program[] = {
		0xBC, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F43, 0x42);
	MCU->CPU.X = 0x10;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDYTest, LDYAbsoluteXWithPageCrosing)
{
	BYTE program[] = {
		0xBC, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3032, 0x42);
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.Y, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}