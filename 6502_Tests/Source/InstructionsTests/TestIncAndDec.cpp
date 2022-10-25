#include "../ComputerTest.h"

class IncDecTest : public ComputerTest {};

TEST_F(IncDecTest, IncDecZeroPage)
{
	BYTE program[] = {
		0xE6, 0x38, 0xE6, 0x38, 0xC6, 0x38
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x38, 0x7E);

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x7E);

	RunCycles(5);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(5);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x80);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(5);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
}

TEST_F(IncDecTest, IncDecZeroPageWithWrapOver)
{
	BYTE program[] = {
		0xE6, 0x38, 0xE6, 0x38, 0xC6, 0x38
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x38, 0xFE);

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0xFE);

	RunCycles(5);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(5);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(5);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
}

TEST_F(IncDecTest, IncDecZeroPageX)
{
	BYTE program[] = {
		0xF6, 0x35, 0xF6, 0x35, 0xD6, 0x35
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x38, 0x7E);
	MCU->CPU.X = 0x03;

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x7E);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x80);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
}

TEST_F(IncDecTest, IncDecZeroPageXWithWrapOverAndPageCrossing)
{
	BYTE program[] = {
		0xF6, 0x39, 0xF6, 0x39, 0xD6, 0x39
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x38, 0xFE);
	MCU->CPU.X = 0xFF;

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0xFE);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x38), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
}


TEST_F(IncDecTest, IncDecAbsolute)
{
	BYTE program[] = {
		0xEE, 0x38, 0x20, 0xEE, 0x38, 0x20, 0xCE, 0x38, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2038, 0x7E);

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x7E);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x80);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
}

TEST_F(IncDecTest, IncDecAbsoluteWithWrapOver)
{
	BYTE program[] = {
		0xEE, 0x38, 0x20, 0xEE, 0x38, 0x20, 0xCE, 0x38, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2038, 0xFE);

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0xFE);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(6);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
}

TEST_F(IncDecTest, IncDecAbsoluteX)
{
	BYTE program[] = {
		0xFE, 0x35, 0x20, 0xFE, 0x35, 0x20, 0xDE, 0x35, 0x20
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2038, 0x7E);
	MCU->CPU.X = 0x03;

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x7E);

	RunCycles(7);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(7);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x80);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(7);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x7F);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);
}

TEST_F(IncDecTest, IncDecAbsoluteXWithWrapOverAndPageCrossing)
{
	BYTE program[] = {
		0xFE, 0x39, 0x1F, 0xFE, 0x39, 0x1F, 0xDE, 0x39, 0x1F
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->SRAM->WriteByte(0x2038, 0xFE);
	MCU->CPU.X = 0xFF;

	RunCycles(8);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0xFE);

	RunCycles(7);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);

	RunCycles(7);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0x00);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 0);

	RunCycles(7);
	EXPECT_EQ(MCU->SRAM->ReadByte(0x2038), 0xFF);
	EXPECT_EQ(MCU->CPU.PS.Bits.Z, 0);
	EXPECT_EQ(MCU->CPU.PS.Bits.N, 1);
}