#include "../ComputerTest.h"

class LDATest : public ComputerTest {};

TEST_F(LDATest, LDAImmediate)
{
	BYTE program[] = {
		0xA9, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

	EXPECT_EQ(MCU->CPU.GetA(), 0x42);
	EXPECT_EQ(MCU->CPU.GetPS().Z, 0);
	EXPECT_EQ(MCU->CPU.GetPS().N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDATest, LDAImmediateWithNegativeValue)
{
	BYTE program[] = {
		0xA9, 0xC2
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	
	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

	EXPECT_EQ(MCU->CPU.GetA(), 0xC2);
	EXPECT_EQ(MCU->CPU.GetPS().Z, 0);
	EXPECT_EQ(MCU->CPU.GetPS().N, 1);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDATest, LDAImmediateWithZeroValue)
{
	BYTE program[] = {
		0xA9, 0x00
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

	EXPECT_EQ(MCU->CPU.GetA(), 0x00);
	EXPECT_EQ(MCU->CPU.GetPS().Z, 1);
	EXPECT_EQ(MCU->CPU.GetPS().N, 0);
	EXPECT_EQ(cycles, 2);
}

TEST_F(LDATest, LDAZeroPage)
{
	BYTE program[] = {
		0xA5, 0xAA
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xAA, 0x42);

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

	EXPECT_EQ(MCU->CPU.GetA(), 0x42);
	EXPECT_EQ(MCU->CPU.GetPS().Z, 0);
	EXPECT_EQ(MCU->CPU.GetPS().N, 0);
	EXPECT_EQ(cycles, 3);
}