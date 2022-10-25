#include "../ComputerTest.h"

class ADCTest : public ComputerTest {};

TEST_F(ADCTest, ADCImmediate)
{
	BYTE program[] = {
		0x69, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x22;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x64);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithZero)
{
	BYTE program[] = {
		0x69, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithCarry)
{
	BYTE program[] = {
		0x69, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xFE;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x40);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithOverflow)
{
	BYTE program[] = {
		0x69, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x84);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithCarryAndOverflow)
{
	BYTE program[] = {
		0x69, 0x86
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x80;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x06);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCZeroPage)
{
	BYTE program[] = {
		0x65, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x35, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(ADCTest, ADCZeroPageX)
{
	BYTE program[] = {
		0x75, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x38, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.X = 0x03;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ADCTest, ADCZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0x75, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x34, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ADCTest, ADCAbsolute)
{
	BYTE program[] = {
		0x6D, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3735, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ADCTest, ADCAbsoluteX)
{
	BYTE program[] = {
		0x7D, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3738, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.X = 0x03;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ADCTest, ADCAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0x7D, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3834, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ADCTest, ADCAbsoluteY)
{
	BYTE program[] = {
		0x79, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3738, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.Y = 0x03;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ADCTest, ADCAbsoluteYWithPageCrossing)
{
	BYTE program[] = {
		0x79, 0x35, 0x37
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3834, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.Y = 0xFF;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ADCTest, ADCIndirectX)
{
	BYTE program[] = {
		0x61, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0037, 0x56);
	MCU->SRAM->WriteByte(0x0038, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.X = 0x04;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ADCTest, ADCIndirectXWithZeroPageCrossover)
{
	BYTE program[] = {
		0x61, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0032, 0x56);
	MCU->SRAM->WriteByte(0x0033, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.X = 0xFF;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ADCTest, ADCIndirectY)
{
	BYTE program[] = {
		0x71, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2059, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.Y = 0x03;
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ADCTest, ADCIndirectYWithPageCrossing)
{
	BYTE program[] = {
		0x71, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2155, 0x42);
	MCU->CPU.A = 0x20;
	MCU->CPU.Y = 0xFF; 
	MCU->CPU.PS.Bits.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x62);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}