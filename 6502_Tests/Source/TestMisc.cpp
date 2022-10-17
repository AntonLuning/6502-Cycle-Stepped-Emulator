#include "ComputerTest.h"

class MiscTest : public ComputerTest {};

TEST_F(MiscTest, CpuCanReset)
{
	MCU->CPU.Reset();

	MCU->CPU.RunCycle(&MCU->SRAM, &MCU->EEPROM);
	EXPECT_EQ(MCU->CPU.AddressBus, 0xFFFC);
	EXPECT_TRUE(MCU->CPU.DataRead);

	MCU->CPU.RunCycle(&MCU->SRAM, &MCU->EEPROM);
	EXPECT_EQ(MCU->CPU.AddressBus, 0xFFFD);
	EXPECT_TRUE(MCU->CPU.DataRead);
}

TEST_F(MiscTest, MemoryCanGetCorrectSizeAndZeroAddress)
{
	MCU->SRAM.ChangeMemory(16 * 1024, 0); // $0000 -> $3FFF

	EXPECT_TRUE(MCU->SRAM.IsAddressOk(0x0000));
	EXPECT_TRUE(MCU->SRAM.IsAddressOk(0x35AF));
	EXPECT_TRUE(MCU->SRAM.IsAddressOk(0x3FFF));
	EXPECT_FALSE(MCU->SRAM.IsAddressOk(0x4000));

	MCU->SRAM.ChangeMemory(16 * 1024, 16 * 1024); // $4000 -> $7FFF
	
	EXPECT_FALSE(MCU->SRAM.IsAddressOk(0x0000));
	EXPECT_FALSE(MCU->SRAM.IsAddressOk(0x35AF));
	EXPECT_FALSE(MCU->SRAM.IsAddressOk(0x3FFF));
	EXPECT_TRUE(MCU->SRAM.IsAddressOk(0x4000));
	EXPECT_TRUE(MCU->SRAM.IsAddressOk(0x55AF));
	EXPECT_TRUE(MCU->SRAM.IsAddressOk(0x7FFF));
	EXPECT_FALSE(MCU->SRAM.IsAddressOk(0x8000));
}

TEST_F(MiscTest, MemoryCanReadAndWrite)
{
	MCU->SRAM.ChangeMemory(16 * 1024, 0); // $0000 -> $3FFF

	EXPECT_EQ(MCU->SRAM.ReadByte(0x0000), 0x00);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x2FAF), 0x00);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x3FFF), 0x00);
	
	MCU->SRAM.WriteByte(0x2FAF, 0x42);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x2FAF), 0x42);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x0000), 0x00);
	MCU->SRAM.Reset();
	EXPECT_EQ(MCU->SRAM.ReadByte(0x2FAF), 0x00);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x0000), 0x00);

	MCU->SRAM.WriteByte(0x0000, 0xAA);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x0000), 0xAA);

	MCU->SRAM.WriteByte(0x3FFF, 0xBB);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x3FFF), 0xBB);

	// If address is not included in memory, nothing should happen
	MCU->SRAM.WriteByte(0x4000, 0xCC);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x4000), 0x00);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x0000), 0xAA);
	EXPECT_EQ(MCU->SRAM.ReadByte(0x3FFF), 0xBB);
}


TEST_F(MiscTest, MemoryCanLoadProgram)
{
	BYTE program[] = {
		0xA9, 0x42
	};
	LoadProgramToEEPROM(program, PROGRAM_LENGTH(program));

	// Start Address
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xFFFC), 0x00);
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xFFFD), 0xC0);

	// Program
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xC000), 0xA9);
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xC001), 0x42);
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xC002), 0xEA);

	// Rest is NOP (0xEA)
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xC003), 0xEA);
	EXPECT_EQ(MCU->EEPROM.ReadByte(0xFFF9), 0xEA);
}

TEST_F(MiscTest, ClockCanStartAndStopAndStep)
{
	MCU->clock.Start();
	EXPECT_TRUE(MCU->clock.IsRunning());

	MCU->clock.Stop();
	EXPECT_FALSE(MCU->clock.IsRunning());

	MCU->clock.Start();
	EXPECT_TRUE(MCU->clock.IsRunning());

	// TODO: Test Step()
}