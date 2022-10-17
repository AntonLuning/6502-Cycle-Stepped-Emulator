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
	
	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

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

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

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

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

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

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

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

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

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
	MCU->SRAM.WriteByte(0x3032, 0x42);
	MCU->CPU.X = 0xFF;

	MCU->clock.Start();

	uint32_t cycles = -3;
	while (MCU->CPU.DataBus != 0xEA)
	{
		cycles++;
		MCU->RunCycle();
	}

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 4);
}