#include "../ComputerTest.h"

class ORATest : public ComputerTest {};

TEST_F(ORATest, ORAImmediate)
{
	BYTE program[] = {
		0x09, 0x42		// 0100 0010
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x33;	// 0011 0011

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x73);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ORATest, ORAImmediateWithZero)
{
	BYTE program[] = {
		0x09, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ORATest, ORAImmediateWithNegative)
{
	BYTE program[] = {
		0x09, 0xF0		// 1111 0000
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x11;	// 0001 0001

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ORATest, ORAZeroPage)
{
	BYTE program[] = {
		0x05, 0xAA
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0xF0);	// 1111 0000
	MCU->CPU.A = 0xC1;					// 1100 0001

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 3);
}

TEST_F(ORATest, ORAZeroPageX)
{
	BYTE program[] = {
		0x15, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xAA, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ORATest, ORAZeroPageXWithZeroPageCrossover)
{
	BYTE program[] = {
		0x15, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0xA7, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ORATest, ORAAbsolute)
{
	BYTE program[] = {
		0x0D, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F33, 0xF0);
	MCU->CPU.A = 0xC1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ORATest, ORAAbsoluteX)
{
	BYTE program[] = {
		0x1D, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F43, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.X = 0x10;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ORATest, ORAAbsoluteXWithPageCrosing)
{
	BYTE program[] = {
		0x1D, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3032, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ORATest, ORAAbsoluteY)
{
	BYTE program[] = {
		0x19, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2F43, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.Y = 0x10;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 4);
}

TEST_F(ORATest, ORAAbsoluteYWithPageCrosing)
{
	BYTE program[] = {
		0x19, 0x33, 0x2F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x3032, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ORATest, ORAIndirectX)
{
	BYTE program[] = {
		0x01, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0037, 0x56);
	MCU->SRAM->WriteByte(0x0038, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.X = 0x04;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ORATest, ORAIndirectXWithZeroPageCrossover)
{
	BYTE program[] = {
		0x01, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0032, 0x56);
	MCU->SRAM->WriteByte(0x0033, 0x20);
	MCU->SRAM->WriteByte(0x2056, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}

TEST_F(ORATest, ORAIndirectY)
{
	BYTE program[] = {
		0x11, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2059, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.Y = 0x03;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ORATest, ORAIndirectYWithPageCrossing)
{
	BYTE program[] = {
		0x11, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x0033, 0x56);
	MCU->SRAM->WriteByte(0x0034, 0x20);
	MCU->SRAM->WriteByte(0x2155, 0xF0);
	MCU->CPU.A = 0xC1;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xF1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 6);
}