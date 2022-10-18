#include "../ComputerTest.h"

class LDXTest : public ComputerTest {};

TEST_F(LDXTest, LDXImmediate)
{
	BYTE program[] = {
		0xA2, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDXTest, LDXImmediateWithNegativeValue)
{
	BYTE program[] = {
		0xA2, 0xC2
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0xC2);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDXTest, LDXImmediateWithZeroValue)
{
	BYTE program[] = {
		0xA2, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDXTest, LDXZeroPage)
{
	BYTE program[] = {
		0xA6, 0xAA
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0xF2);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0xF2);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 3);
}

TEST_F(LDXTest, LDXZeroPageY)
{
	BYTE program[] = {
		0xB6, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0x42);
	MCU->CPU.Y = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDXTest, LDXZeroPageYWithZeroPageCrossover)
{
	BYTE program[] = {
		0xB6, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xA7, 0x42);
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDXTest, LDXAbsolute)
{
	BYTE program[] = {
		0xAE, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F33, 0x42);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDXTest, LDXAbsoluteY)
{
	BYTE program[] = {
		0xBE, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F43, 0x42);
	MCU->CPU.Y = 0x10;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDXTest, LDXAbsoluteYWithPageCrosing)
{
	BYTE program[] = {
		0xBE, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3032, 0x42);
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}