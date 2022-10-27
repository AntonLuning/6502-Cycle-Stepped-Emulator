#include "../ComputerTest.h"

class BITTest : public ComputerTest {};

TEST_F(BITTest, BITZeroPage)
{
	BYTE program[] = {
		0x24, 0x36
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x36, 0x32);	// 0011 0010
	MCU->CPU.A = 0x22;					// 0010 0010

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x22);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BITTest, BITZeroPageWithOverflowAndNegativeFlags)
{
	BYTE program[] = {
		0x24, 0x36
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x36, 0xC2);	// 1100 0010
	MCU->CPU.A = 0x22;					// 0010 0010

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x22);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BITTest, BITZeroPageWithZeroFlag)
{
	BYTE program[] = {
		0x24, 0x36
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x36, 0x3D);	// 0011 1101
	MCU->CPU.A = 0xC2;					// 1100 0010

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xC2);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BITTest, BITAbsolute)
{
	BYTE program[] = {
		0x2C, 0x36, 0x12
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x1236, 0x32);	// 0011 0010
	MCU->CPU.A = 0x22;					// 0010 0010

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x22);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}