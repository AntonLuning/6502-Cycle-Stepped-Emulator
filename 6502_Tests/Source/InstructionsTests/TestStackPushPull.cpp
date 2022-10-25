#include "../ComputerTest.h"

class StackPushPullTest : public ComputerTest {};

TEST_F(StackPushPullTest, PHARegular)
{
	BYTE program[] = {
		0x48
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x42;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FD), 0x42);
	EXPECT_EQ(MCU->CPU.SP, 0xFC);
	EXPECT_EQ(cycles, 3);
}

TEST_F(StackPushPullTest, PHAZero)
{
	BYTE program[] = {
		0x48
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.A = 0x00;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FD), 0x00);
	EXPECT_EQ(MCU->CPU.SP, 0xFC);
	EXPECT_EQ(cycles, 3);
}

TEST_F(StackPushPullTest, PHPRegular)
{
	BYTE program[] = {
		0x08
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.PS.Byte = 0x00;
	MCU->CPU.PS.Bits.D = 1;
	MCU->CPU.PS.Bits.Z = 1;
	MCU->CPU.PS.Bits.I = 1;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Bits.D, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FD), MCU->CPU.PS.Byte);
	EXPECT_EQ(MCU->CPU.SP, 0xFC);
	EXPECT_EQ(cycles, 3);
}

TEST_F(StackPushPullTest, PHPZero)
{
	BYTE program[] = {
		0x08
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.PS.Byte = 0x00;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Byte, 0x04);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FD), 0x04);
	EXPECT_EQ(MCU->CPU.SP, 0xFC);
	EXPECT_EQ(cycles, 3);
}

TEST_F(StackPushPullTest, PLARegular)
{
	BYTE program[] = {
		0x68
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x01FE, 0x42);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x42);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FE), 0x42);
	EXPECT_EQ(MCU->CPU.SP, 0xFE);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(StackPushPullTest, PLAZero)
{
	BYTE program[] = {
		0x68
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x01FE, 0x00);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.A, 0x00);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FE), 0x00);
	EXPECT_EQ(MCU->CPU.SP, 0xFE);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
	EXPECT_EQ(cycles, 4);
}

TEST_F(StackPushPullTest, PLPRegular)
{
	BYTE program[] = {
		0x28
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x01FE, 0x40);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Byte, 0x40);
	EXPECT_EQ(MCU->CPU.PS.Bits.V, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FE), 0x40);
	EXPECT_EQ(MCU->CPU.SP, 0xFE);
	EXPECT_EQ(cycles, 4);
}

TEST_F(StackPushPullTest, PLPZero)
{
	BYTE program[] = {
		0x28
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x01FE, 0x00);

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.PS.Byte, 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.D, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.UB, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.C, 0);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x01FE), 0x00);
	EXPECT_EQ(MCU->CPU.SP, 0xFE);
	EXPECT_EQ(cycles, 4);
}