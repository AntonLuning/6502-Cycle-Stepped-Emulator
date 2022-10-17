#include "ComputerTest.h"

class ProgramTest : public ComputerTest {};

TEST_F(ProgramTest, CanRunProgram1)
{
	BYTE program[] = {
		0xA9, 0x42,		// LDA Immediate
		0xA5, 0x20		// LDA Zero Page
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
	EXPECT_EQ(cycles, 5);
}