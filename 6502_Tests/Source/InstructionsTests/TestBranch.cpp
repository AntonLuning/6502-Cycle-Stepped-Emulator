#include "../ComputerTest.h"

class BranchTest : public ComputerTest {};

TEST_F(BranchTest, BCCIgnore)
{
	BYTE program[] = {
		0xE8, 0x38, 0x90, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x04);
	EXPECT_EQ(cycles, 10);
}

TEST_F(BranchTest, BCCForward)
{
	BYTE program[] = {
		0xE8, 0x18, 0x90, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(cycles, 7);
}

TEST_F(BranchTest, BCCBackward)
{
	BYTE program[] = {
		0xE8, 0x18, 0x90, 0xFC,
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC001);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC002);

	RunCycles(3);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC000);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(MCU->CPU.PC, 0xC001);
}

TEST_F(BranchTest, BCCForwardWithPageCrossing)
{
	BYTE program[] = {
		0x4C, 0xF0, 0xC0
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xC0F0, 0xE8);
	MCU->EEPROM->WriteByte(0xC0F1, 0x18);
	MCU->EEPROM->WriteByte(0xC0F2, 0x90);
	MCU->EEPROM->WriteByte(0xC0F3, 0x10);

	MCU->EEPROM->WriteByte(0xC104, 0xE8);
	MCU->EEPROM->WriteByte(0xC105, 0xE8);
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x04);
	EXPECT_EQ(cycles, 15);
}

TEST_F(BranchTest, BCCBackWardWithPageCrossing)
{
	BYTE program[] = {
		0x4C, 0x04, 0xC1
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xC104, 0xE8);
	MCU->EEPROM->WriteByte(0xC105, 0x18);
	MCU->EEPROM->WriteByte(0xC106, 0x90);
	MCU->EEPROM->WriteByte(0xC107, 0xF0);

	MCU->EEPROM->WriteByte(0xC0F8, 0xE8);
	MCU->EEPROM->WriteByte(0xC0F9, 0xE8);
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x04);
	EXPECT_EQ(cycles, 15);
}

TEST_F(BranchTest, BCSIgnore)
{
	BYTE program[] = {
		0xE8, 0x18, 0xB0, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x04);
	EXPECT_EQ(cycles, 10);
}

TEST_F(BranchTest, BCSBranch)
{
	BYTE program[] = {
		0xE8, 0x38, 0xB0, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(cycles, 7);
}

TEST_F(BranchTest, BNEIgnore)
{
	BYTE program[] = {
		0xD0, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.Z = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 6);
}

TEST_F(BranchTest, BNEBranch)
{
	BYTE program[] = {
		0xD0, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.Z = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BranchTest, BEQIgnore)
{
	BYTE program[] = {
		0xF0, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.Z = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 6);
}

TEST_F(BranchTest, BEQBranch)
{
	BYTE program[] = {
		0xF0, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.Z = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BranchTest, BPLIgnore)
{
	BYTE program[] = {
		0x10, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.N = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 6);
}

TEST_F(BranchTest, BPLBranch)
{
	BYTE program[] = {
		0x10, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.N = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BranchTest, BMIIgnore)
{
	BYTE program[] = {
		0x30, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.N = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 6);
}

TEST_F(BranchTest, BMIBranch)
{
	BYTE program[] = {
		0x30, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.N = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BranchTest, BVCIgnore)
{
	BYTE program[] = {
		0x50, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.V = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 6);
}

TEST_F(BranchTest, BVCBranch)
{
	BYTE program[] = {
		0x50, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.V = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(cycles, 3);
}

TEST_F(BranchTest, BVSIgnore)
{
	BYTE program[] = {
		0x70, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.V = 0;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(cycles, 6);
}

TEST_F(BranchTest, BVSBranch)
{
	BYTE program[] = {
		0x70, 0x02, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.V = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(cycles, 3);
}