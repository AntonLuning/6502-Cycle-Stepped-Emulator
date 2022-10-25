#include "../ComputerTest.h"

class JMPTest : public ComputerTest {};

TEST_F(JMPTest, JMPAbsolute)
{
	BYTE program[] = {
		0xE8, 0x4C, 0x06, 0xC0, 0xE8, 0xE8, 0xE8,
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 7);
}

TEST_F(JMPTest, JMPIndirect)
{
	BYTE program[] = {
		0xE8, 0x6C, 0x00, 0xE0, 0xE8, 0xE8, 0xE8,
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xE000, 0x06);
	MCU->EEPROM->WriteByte(0xE001, 0xC0);
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 9);
}

TEST_F(JMPTest, JMPToProgramEnd)
{
	BYTE program[] = {
		0xE8, 0x4C, 0x0A, 0xC0, 0xE8, 0xE8, 0xE8,
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(cycles, 5);
}

TEST_F(JMPTest, JMPToProgramStart)
{
	BYTE program[] = {
		0xE8, 0x4C, 0x00, 0xC0
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	RunCycles(8);

	RunCycles(5);
	EXPECT_EQ(MCU->CPU.X, 0x02);

	RunCycles(5);
	EXPECT_EQ(MCU->CPU.X, 0x03);

	RunCycles(5);
	EXPECT_EQ(MCU->CPU.X, 0x04);

	RunCycles(5);
	EXPECT_EQ(MCU->CPU.X, 0x05);
}