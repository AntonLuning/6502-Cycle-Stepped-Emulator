#include "../ComputerTest.h"

class STATest : public ComputerTest {};

TEST_F(STATest, STAZeroPage)
{
	BYTE program[] = {
		0x85, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0xA8), 0x42);
	EXPECT_EQ(cycles, 3);
}

TEST_F(STATest, STAZeroPageX)
{
	BYTE program[] = {
		0x95, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0x03;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0xA8), 0x00);
	EXPECT_EQ(MCU->SRAM.ReadByte(0xAB), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STATest, STAZeroPageXWithZeroPageCrossing)
{
	BYTE program[] = {
		0x95, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0xA8), 0x00);
	EXPECT_EQ(MCU->SRAM.ReadByte(0xA7), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STATest, STAAbsolute)
{
	BYTE program[] = {
		0x8D, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x33A8), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STATest, STAAbsoluteX)
{
	BYTE program[] = {
		0x9D, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0x03;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x33AB), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STATest, STAAbsoluteXWithPageCrossing)
{
	BYTE program[] = {
		0x9D, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x34A7), 0x42);
	EXPECT_EQ(cycles, 5);
}

TEST_F(STATest, STAAbsoluteY)
{
	BYTE program[] = {
		0x99, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0x03;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x33AB), 0x42);
	EXPECT_EQ(cycles, 4);
}

TEST_F(STATest, STAAbsoluteYWithPageCrossing)
{
	BYTE program[] = {
		0x99, 0xA8, 0x33
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x34A7), 0x42);
	EXPECT_EQ(cycles, 5);
}

TEST_F(STATest, STAIndirectX)
{
	BYTE program[] = {
		0x81, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xAA, 0xF7);
	MCU->SRAM.WriteByte(0xAB, 0x33);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0x02;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x33F7), 0x42);
	EXPECT_EQ(cycles, 6);
}

TEST_F(STATest, STAIndirectXWitZeroPageCrossing)
{
	BYTE program[] = {
		0x81, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xA7, 0xF7);
	MCU->SRAM.WriteByte(0xA8, 0x33);
	MCU->CPU.A = 0x42;
	MCU->CPU.X = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x33F7), 0x42);
	EXPECT_EQ(cycles, 6);
}

TEST_F(STATest, STAIndirectY)
{
	BYTE program[] = {
		0x91, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xA8, 0xF7);
	MCU->SRAM.WriteByte(0xA9, 0x33);
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0x02;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x33F9), 0x42);
	EXPECT_EQ(cycles, 5);
}

TEST_F(STATest, STAIndirectYWithPageCrossing)
{
	BYTE program[] = {
		0x91, 0xA8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM.WriteByte(0xA8, 0xF7);
	MCU->SRAM.WriteByte(0xA9, 0x33);
	MCU->CPU.A = 0x42;
	MCU->CPU.Y = 0xFF;

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->SRAM.ReadByte(0x34F6), 0x42);
	EXPECT_EQ(cycles, 6);
}