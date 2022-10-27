#include "../ComputerTest.h"

class JSRAndRTSTest : public ComputerTest {};

TEST_F(JSRAndRTSTest, JSRAndRTS)
{
	BYTE program[] = {
		0xE8, 0x20, 0x00, 0xF0
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xF000, 0xC8);
	MCU->EEPROM->WriteByte(0xF001, 0x60);
	MCU->CPU.X = 0x01;
	MCU->CPU.Y = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x02);
	EXPECT_EQ(MCU->CPU.PC, 0xC004);
	EXPECT_EQ(cycles, 16);
}

TEST_F(JSRAndRTSTest, JSRAndRTSMultiple)
{
	BYTE program[] = {
		0xE8, 0x20, 0x00, 0xF0, 0x20, 0x00, 0xF0
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xF000, 0xC8);
	MCU->EEPROM->WriteByte(0xF001, 0x60);
	MCU->CPU.X = 0x01;
	MCU->CPU.Y = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x02);
	EXPECT_EQ(MCU->CPU.Y, 0x03);
	EXPECT_EQ(MCU->CPU.PC, 0xC007);
	EXPECT_EQ(cycles, 30);
}

TEST_F(JSRAndRTSTest, JSRAndRTSNested)
{
	BYTE program[] = {
		0xE8, 0x20, 0x00, 0xF0, 0xE8
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));
	MCU->EEPROM->WriteByte(0xF000, 0xC8);
	MCU->EEPROM->WriteByte(0xF001, 0x20);
	MCU->EEPROM->WriteByte(0xF002, 0x00);
	MCU->EEPROM->WriteByte(0xF003, 0xF1);
	MCU->EEPROM->WriteByte(0xF004, 0xC8);
	MCU->EEPROM->WriteByte(0xF005, 0x60);

	MCU->EEPROM->WriteByte(0xF100, 0xC8);
	MCU->EEPROM->WriteByte(0xF101, 0x60);

	MCU->CPU.X = 0x01;
	MCU->CPU.Y = 0x01;

	int32_t cycles = RunTestProgram();

	EXPECT_EQ(MCU->CPU.X, 0x03);
	EXPECT_EQ(MCU->CPU.Y, 0x04);
	EXPECT_EQ(MCU->CPU.PC, 0xC005);
	EXPECT_EQ(cycles, 34);
}