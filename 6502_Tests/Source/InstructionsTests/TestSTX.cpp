#include "../ComputerTest.h"

class STXTest : public ComputerTest {};

TEST_F(STXTest, STXZeroPage)
{
	BYTE program[] = {
		0x86, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0xA8), 0x42);
	EXPECT_EQ(cycles, 3);
}

TEST_F(STXTest, STXZeroPageY)
{
	BYTE program[] = {
		0x96, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;
	MCU->CPU.Y = 0x03;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0xA8), 0x00);
	EXPECT_EQ(MCU->SRAM->ReadByte(0xAB), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STXTest, STXZeroPageYWithZeroPageCrossing)
{
	BYTE program[] = {
		0x96, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;
	MCU->CPU.Y = 0xFF;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0xA8), 0x00);
	EXPECT_EQ(MCU->SRAM->ReadByte(0xA7), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STXTest, STXAbsolute)
{
	BYTE program[] = {
		0x8E, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM->ReadByte(0x33A8), 0x42);
	EXPECT_EQ(cycles, 4);
}