#include "../ComputerTest.h"

class LDATest : public ComputerTest {};

TEST_F(LDATest, LDAImmediate)
{
	BYTE program[] = {
		0xA9, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDATest, LDAImmediateWithNegativeValue)
{
	BYTE program[] = {
		0xA9, 0xC2
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	
	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xC2);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDATest, LDAImmediateWithZeroValue)
{
	BYTE program[] = {
		0xA9, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDATest, LDAZeroPage)
{
	BYTE program[] = {
		0xA5, 0xAA
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xAA, 0xF2);

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF2);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 3);
}

TEST_F(LDATest, LDAZeroPageX)
{
	BYTE program[] = {
		0xB5, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xAA, 0x42);
	MCU->CPU.X = 0x02;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDATest, LDAZeroPageXWithZeroPageCrossover)
{
	BYTE program[] = {
		0xB5, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xA7, 0x42);
	MCU->CPU.X = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDATest, LDAAbsolute)
{
	BYTE program[] = {
		0xAD, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x2F33, 0x42);

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDATest, LDAAbsoluteX)
{
	BYTE program[] = {
		0xBD, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x2F43, 0x42);
	MCU->CPU.X = 0x10;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDATest, LDAAbsoluteXWithPageCrosing)
{
	BYTE program[] = {
		0xBD, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x3032, 0x42);
	MCU->CPU.X = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(LDATest, LDAAbsoluteY)
{
	BYTE program[] = {
		0xB9, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x2F43, 0x42);
	MCU->CPU.Y = 0x10;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(LDATest, LDAAbsoluteYWithPageCrosing)
{
	BYTE program[] = {
		0xB9, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x3032, 0x42);
	MCU->CPU.Y = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(LDATest, LDAIndirectX)
{
	BYTE program[] = {
		0xA1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x0037, 0x56);
	MCU->SRAM.WriteByte(0x0038, 0x20);
	MCU->SRAM.WriteByte(0x2056, 0x42);
	MCU->CPU.X = 0x04;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(LDATest, LDAIndirectXWithZeroPageCrossover)
{
	BYTE program[] = {
		0xA1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x0032, 0x56);
	MCU->SRAM.WriteByte(0x0033, 0x20);
	MCU->SRAM.WriteByte(0x2056, 0x42);
	MCU->CPU.X = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(LDATest, LDAIndirectY)
{
	BYTE program[] = {
		0xB1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x0033, 0x56);
	MCU->SRAM.WriteByte(0x0034, 0x20);
	MCU->SRAM.WriteByte(0x2059, 0x42);
	MCU->CPU.Y = 0x03;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(LDATest, LDAIndirectYWithPageCrossing)
{
	BYTE program[] = {
		0xB1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0x0033, 0x56);
	MCU->SRAM.WriteByte(0x0034, 0x20);
	MCU->SRAM.WriteByte(0x2155, 0x42);
	MCU->CPU.Y = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 6);
}