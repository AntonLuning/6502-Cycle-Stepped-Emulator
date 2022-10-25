#include "../ComputerTest.h"

class PSFlagsTest : public ComputerTest {};

TEST_F(PSFlagsTest, CarryFlag)
{
	BYTE program[] = {
		0x18, 0x38, 0x18
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
}

TEST_F(PSFlagsTest, DecimalFlag)
{
	BYTE program[] = {
		0xD8, 0xF8, 0xD8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.D, 0);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.D, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.D, 0);
}

TEST_F(PSFlagsTest, InterruptFlag)
{
	BYTE program[] = {
		0x58, 0x78, 0x58
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 0);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 0);
}

TEST_F(PSFlagsTest, OverflowFlag)
{
	BYTE program[] = {
		0xB8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	RunCycles(8);

	MCU->CPU.PS.Bits.V = 1;
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 0);
}