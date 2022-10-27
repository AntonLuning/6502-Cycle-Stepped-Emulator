#include "../ComputerTest.h"

class CompareTest : public ComputerTest {};

TEST_F(CompareTest, CMPImmediate)
{
	BYTE program[] = {
		0xC9, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(CompareTest, CMPImmediateWithSameValue)
{
	BYTE program[] = {
		0xC9, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}


TEST_F(CompareTest, CMPImmediateWithNegativeResult)
{
	BYTE program[] = {
		0xC9, 0xAD
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(CompareTest, CMPZeroPage)
{
	BYTE program[] = {
		0xC5, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x42);
	MCU->CPU.A = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(CompareTest, CMPZeroPageX)
{
	BYTE program[] = {
		0xD5, 0x18
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(CompareTest, CMPZeroPageXWithPageCrossing)
{
	BYTE program[] = {
		0xD5, 0x21
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(CompareTest, CMPAbsolute)
{
	BYTE program[] = {
		0xCD, 0x20, 0x15
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1520, 0x42);
	MCU->CPU.A = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(CompareTest, CMPAbsoluteX)
{
	BYTE program[] = {
		0xDD, 0x18, 0x15
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x151A, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(CompareTest, CMPAbsoluteXWithPageCrossover)
{
	BYTE program[] = {
		0xDD, 0x21, 0x14
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1520, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(CompareTest, CMPAbsoluteY)
{
	BYTE program[] = {
		0xD9, 0x18, 0x15
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x151A, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(CompareTest, CMPAbsoluteYWithPageCrossover)
{
	BYTE program[] = {
		0xD9, 0x21, 0x14
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1520, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(CompareTest, CMPIndirectX)
{
	BYTE program[] = {
		0xC1, 0x18
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1A, 0x15);
	MCU->SRAM->WriteByte(0x1B, 0x33);
	MCU->SRAM->WriteByte(0x3315, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(CompareTest, CMPIndirectXWithPageCrossover)
{
	BYTE program[] = {
		0xC1, 0x21
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x15);
	MCU->SRAM->WriteByte(0x21, 0x33);
	MCU->SRAM->WriteByte(0x3315, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}

TEST_F(CompareTest, CMPIndirectY)
{
	BYTE program[] = {
		0xD1, 0x18
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x18, 0x20);
	MCU->SRAM->WriteByte(0x19, 0x15);
	MCU->SRAM->WriteByte(0x1522, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0x02;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(CompareTest, CMPIndirectYWithPageCrossover)
{
	BYTE program[] = {
		0xD1, 0x18
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x18, 0x20);
	MCU->SRAM->WriteByte(0x19, 0x15);
	MCU->SRAM->WriteByte(0x161F, 0x42);
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 6);
}


TEST_F(CompareTest, CPXImmediate)
{
	BYTE program[] = {
		0xE0, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(CompareTest, CPXImmediateWithSameValue)
{
	BYTE program[] = {
		0xE0, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}


TEST_F(CompareTest, CPXImmediateWithNegativeResult)
{
	BYTE program[] = {
		0xE0, 0xAD
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(CompareTest, CPXZeroPage)
{
	BYTE program[] = {
		0xE4, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x42);
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(CompareTest, CPXAbsolute)
{
	BYTE program[] = {
		0xEC, 0x20, 0x15
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1520, 0x42);
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}


TEST_F(CompareTest, CPYImmediate)
{
	BYTE program[] = {
		0xC0, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(CompareTest, CPYImmediateWithSameValue)
{
	BYTE program[] = {
		0xC0, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 2);
}


TEST_F(CompareTest, CPYImmediateWithNegativeResult)
{
	BYTE program[] = {
		0xC0, 0xAD
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(CompareTest, CPYZeroPage)
{
	BYTE program[] = {
		0xC4, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x20, 0x42);
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(CompareTest, CPYAbsolute)
{
	BYTE program[] = {
		0xCC, 0x20, 0x15
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1520, 0x42);
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}