#include "ComputerTest.h"

class ProgramTest : public ComputerTest {};

TEST_F(ProgramTest, CanRunProgramMultipleLDA)
{
	BYTE program[] = {
		0xA9, 0x42,		// LDA Immediate
		0xA5, 0x20		// LDA Zero Page
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ProgramTest, CanRunProgramLDAAndSTA)
{
	BYTE program[] = {
		0xA9, 0x42,		// LDA Immediate
		0x85, 0x20,		// LDA Zero Page
		0xA9, 0xFA,		// LDA Immediate
		0x85, 0x66		// LDA Zero Page
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	uint32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xFA);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x20), 0x42);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x66), 0xFA);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 10);
}