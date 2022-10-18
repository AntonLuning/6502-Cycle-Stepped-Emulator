#include "../ComputerTest.h"

class STYTest : public ComputerTest {};

TEST_F(STYTest, STYZeroPage)
{
	BYTE program[] = {
		0x84, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0xA8), 0x42);
	EXPECT_EQ(cycles, 3);
}

TEST_F(STYTest, STYZeroPageX)
{
	BYTE program[] = {
		0x94, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;
	MCU->CPU.X = 0x03;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0xA8), 0x00);
	EXPECT_EQ(MCU->SRAM->ReadByte(0xAB), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STYTest, STYZeroPageXWithZeroPageCrossing)
{
	BYTE program[] = {
		0x94, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;
	MCU->CPU.X = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0xA8), 0x00);
	EXPECT_EQ(MCU->SRAM->ReadByte(0xA7), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STYTest, STYAbsolute)
{
	BYTE program[] = {
		0x8C, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.Y = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x33A8), 0x42);
	EXPECT_EQ(cycles, 4);
}