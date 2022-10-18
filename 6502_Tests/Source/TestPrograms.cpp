#include "ComputerTest.h"

class ProgramTest : public ComputerTest {};

TEST_F(ProgramTest, ProgramMultipleLDA)
{
	BYTE program[] = {
		0xA9, 0x42,		// LDA Immediate
		0xA5, 0x20		// LDA Zero Page
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 5);
}

TEST_F(ProgramTest, ProgramLDAAndSTA)
{
	BYTE program[] = {
		0xA9, 0x42,		// LDA Immediate
		0x85, 0x20,		// STA Zero Page
		0xA9, 0xFA,		// LDA Immediate
		0x85, 0x66		// STA Zero Page
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0xFA);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x20), 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x66), 0xFA);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 1);
	EXPECT_EQ(cycles, 10);
}

TEST_F(ProgramTest, ProgramLDAndSTWithAXY)
{
	BYTE program[] = {
		0xA2, 0x05,			// LDX Immediate
		0xA9, 0x42,			// LDA Immediate
		0x95, 0x20,			// STA Zero Page X
		0xA0, 0xFF,			// LDY Immediate
		0xB6, 0x26,			// LDX Zero Page Y
		0x8E, 0x66, 0x11	// STX Absolute
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->CPU.X, 0x42);
	EXPECT_EQ(MCU->CPU.Y, 0xFF);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x25), 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x1166), 0x42);
	EXPECT_EQ(MCU->CPU.PS.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.N, 0);
	EXPECT_EQ(cycles, 18);
}