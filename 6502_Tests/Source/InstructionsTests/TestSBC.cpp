#include "../ComputerTest.h"

class SBCTest : public ComputerTest {};

TEST_F(SBCTest, SBCImmediate)
{
	BYTE program[] = {
		0xE9, 0x22
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(SBCTest, SBCImmediateWithZero)
{
	BYTE program[] = {
		0xE9, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(SBCTest, SBCImmediateWithCarry)
{
	BYTE program[] = {
		0xE9, 0xFF
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x22;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x23);
	EXPECT_EQ(MCU->CPU.PS.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(SBCTest, SBCImmediateWithOverflow)
{
	BYTE program[] = {
		0xE9, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x82;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x40);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(SBCTest, SBCImmediateWithCarryAndOverflow)
{
	BYTE program[] = {
		0xE9, 0x86
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x76;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF0);
	EXPECT_EQ(MCU->CPU.PS.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(SBCTest, SBCZeroPage)
{
	BYTE program[] = {
		0xE5, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x35, 0x42);
	MCU->CPU.A = 0x92;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x50);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(SBCTest, SBCZeroPageX)
{
	BYTE program[] = {
		0xF5, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x38, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.X = 0x03;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(SBCTest, SBCZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0xF5, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x34, 0x42);
	MCU->CPU.A = 0x22;
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xE0);
	EXPECT_EQ(MCU->CPU.PS.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(SBCTest, SBCAbsolute)
{
	BYTE program[] = {
		0xED, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3735, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(SBCTest, SBCAbsoluteX)
{
	BYTE program[] = {
		0xFD, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3738, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.X = 0x03;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(SBCTest, SBCAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0xFD, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3834, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(SBCTest, SBCAbsoluteY)
{
	BYTE program[] = {
		0xF9, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3738, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.Y = 0x03;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(SBCTest, SBCAbsoluteYWithPageCrossing)
{
	BYTE program[] = {
		0xF9, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3834, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.Y = 0xFF;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(SBCTest, SBCIndirectX)
{
	BYTE program[] = {
		0xE1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0037, 0x56);
	MCU->SRAM->WriteByte(0x0038, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.X = 0x04;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(SBCTest, SBCIndirectXWithZeroPageCrossover)
{
	BYTE program[] = {
		0xE1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0032, 0x56);
	MCU->SRAM->WriteByte(0x0033, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(SBCTest, SBCIndirectY)
{
	BYTE program[] = {
		0xF1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2059, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.Y = 0x03;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(SBCTest, SBCIndirectYWithPageCrossing)
{
	BYTE program[] = {
		0xF1, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2155, 0x42);
	MCU->CPU.A = 0x62;
	MCU->CPU.Y = 0xFF;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x20);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 6);
}