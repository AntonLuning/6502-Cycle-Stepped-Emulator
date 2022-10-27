#include "../ComputerTest.h"

class EORTest : public ComputerTest {};

TEST_F(EORTest, EORImmediate)
{
	BYTE program[] = {
		0x49, 0x42		// 0100 0010
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x33;	// 0011 0011

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x71);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(EORTest, EORImmediateWithZero)
{
	BYTE program[] = {
		0x49, 0xF3
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xF3;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(EORTest, EORImmediateWithNegative)
{
	BYTE program[] = {
		0x49, 0xF0		// 1111 0000
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x11;	// 0001 0001

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xE1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(EORTest, EORZeroPage)
{
	BYTE program[] = {
		0x45, 0xAA
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0xF0);	// 1111 0000
	MCU->CPU.A = 0x51;					// 0101 0001

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 3);
}

TEST_F(EORTest, EORZeroPageX)
{
	BYTE program[] = {
		0x55, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(EORTest, EORZeroPageXWithZeroPageCrossover)
{
	BYTE program[] = {
		0x55, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xA7, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(EORTest, EORAbsolute)
{
	BYTE program[] = {
		0x4D, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F33, 0xF0);
	MCU->CPU.A = 0x51;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(EORTest, EORAbsoluteX)
{
	BYTE program[] = {
		0x5D, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F43, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.X = 0x10;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(EORTest, EORAbsoluteXWithPageCrosing)
{
	BYTE program[] = {
		0x5D, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3032, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(EORTest, EORAbsoluteY)
{
	BYTE program[] = {
		0x59, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F43, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.Y = 0x10;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(EORTest, EORAbsoluteYWithPageCrosing)
{
	BYTE program[] = {
		0x59, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3032, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(EORTest, EORIndirectX)
{
	BYTE program[] = {
		0x41, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0037, 0x56);
	MCU->SRAM->WriteByte(0x0038, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.X = 0x04;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(EORTest, EORIndirectXWithZeroPageCrossover)
{
	BYTE program[] = {
		0x41, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0032, 0x56);
	MCU->SRAM->WriteByte(0x0033, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(EORTest, EORIndirectY)
{
	BYTE program[] = {
		0x51, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2059, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.Y = 0x03;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(EORTest, EORIndirectYWithPageCrossing)
{
	BYTE program[] = {
		0x51, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2155, 0xF0);
	MCU->CPU.A = 0x51;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xA1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}