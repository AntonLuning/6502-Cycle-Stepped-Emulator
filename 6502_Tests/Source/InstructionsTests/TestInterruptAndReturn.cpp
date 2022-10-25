#include "../ComputerTest.h"

class InterruptAndReturnTest : public ComputerTest {};

TEST_F(InterruptAndReturnTest, BRK)
{
	BYTE program[] = {
		0x00, 0x02	// 0x02 is illegal opcode (used as padding byte here)
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xFFFE, 0x00);
	MCU->EEPROM->WriteByte(0xFFFF, 0xF0);
	MCU->EEPROM->WriteByte(0xF000, 0xE8);
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xF001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.B, 1);
	EXPECT_EQ(cycles, 9);
}

TEST_F(InterruptAndReturnTest, BRKWithRTI)
{
	BYTE program[] = {
		0x00, 0x02
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xFFFE, 0x00);
	MCU->EEPROM->WriteByte(0xFFFF, 0xF0);
	MCU->EEPROM->WriteByte(0xF000, 0xE8);
	MCU->EEPROM->WriteByte(0xF001, 0x40);
	MCU->EEPROM->WriteByte(0xF002, 0x02);
	MCU->CPU.X = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC002);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);
	EXPECT_EQ(MCU->CPU.PS.Bits.B, 0);
	EXPECT_EQ(cycles, 15);
}

TEST_F(InterruptAndReturnTest, IRQWithRTI)
{
	BYTE program[] = {
		0x58, 0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xFFFE, 0x00);
	MCU->EEPROM->WriteByte(0xFFFF, 0xF0);
	MCU->EEPROM->WriteByte(0xF000, 0xC8);
	MCU->EEPROM->WriteByte(0xF001, 0x40);
	MCU->EEPROM->WriteByte(0xF002, 0x02);
	MCU->CPU.X = 0x01;
	MCU->CPU.Y = 0x01;

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x01);
	EXPECT_EQ(MCU->CPU.PC, 0xC001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 0);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC002);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 0);

	MCU->CPU.InterruptIRQ();

	RunCycles(7);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x01);
	EXPECT_EQ(MCU->CPU.PC, 0xF000);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xF001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	RunCycles(6);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC002);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 0);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC003);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 0);
}

TEST_F(InterruptAndReturnTest, IRQWithoutClearingInterruptFlag)
{
	BYTE program[] = {
		0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->CPU.X = 0x01;
	MCU->CPU.PS.Bits.I = 1;

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	MCU->CPU.InterruptIRQ();

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(MCU->CPU.PC, 0xC002);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);
}

TEST_F(InterruptAndReturnTest, NMIWithRTI)
{
	BYTE program[] = {
		0xE8, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xFFFA, 0x00);
	MCU->EEPROM->WriteByte(0xFFFB, 0xF0);
	MCU->EEPROM->WriteByte(0xF000, 0xC8);
	MCU->EEPROM->WriteByte(0xF001, 0x40);
	MCU->EEPROM->WriteByte(0xF002, 0x02);
	MCU->CPU.X = 0x01;
	MCU->CPU.Y = 0x01;
	MCU->CPU.PS.Bits.I = 1;

	RunCycles(8);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	MCU->CPU.InterruptNMI();

	RunCycles(7);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x01);
	EXPECT_EQ(MCU->CPU.PC, 0xF000);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xF001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	RunCycles(6);
	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC001);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);

	RunCycles(2);
	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC002);
	EXPECT_EQ(MCU->CPU.PS.Bits.I, 1);
}