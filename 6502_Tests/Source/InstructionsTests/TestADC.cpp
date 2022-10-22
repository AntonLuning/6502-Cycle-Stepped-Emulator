#include "../ComputerTest.h"

class ADCTest : public ComputerTest {};

TEST_F(ADCTest, ADCImmediate)
{
	BYTE program[] = {
		0x69, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x22;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x64);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithZero)
{
	BYTE program[] = {
		0x69, 0x00
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

TEST_F(ADCTest, ADCImmediateWithCarry)
{
	BYTE program[] = {
		0x69, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0xFE;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x40);
	EXPECT_EQ(MCU->CPU.PS.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithOverflow)
{
	BYTE program[] = {
		0x69, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x84);
	EXPECT_EQ(MCU->CPU.PS.C, 0);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(ADCTest, ADCImmediateWithoutOverflow)
{
	BYTE program[] = {
		0x69, 0x86
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x80;
	MCU->CPU.PS.C = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x06);
	EXPECT_EQ(MCU->CPU.PS.C, 1);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.V, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 2);
}