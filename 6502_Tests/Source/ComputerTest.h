#pragma once

#include <Log.h>
#include <Computer.h>
#include <OperationCodes.h>

#include "gtest/gtest.h"

#define PROGRAM_LENGTH(prg) sizeof(prg) / sizeof(BYTE)

constexpr uint32_t SRAM_MEMORY = 32 * 1024;
constexpr uint32_t EEPROM_MEMORY = 16 * 1024;

// $0000 -> $7FFF : SRAM
// $C000 -> $FFFF : EEPROM
// $8000 -> $BFFF : free addresses

class ComputerTest : public ::testing::Test
{
protected:
	Computer* MCU = nullptr;

	void SetUp() override
	{
		Log::Init();

		MCU = new Computer(SRAM_MEMORY, EEPROM_MEMORY);
		MCU->clock.SetSpeedMS(1);
		MCU->clock.Start();
	}

	virtual void TearDown()
	{
		delete MCU;
	}

	void LoadProgramToEEPROM(BYTE program[], uint32_t programLength)
	{
		BYTE* prg = new BYTE[EEPROM_MEMORY];

		for (int i = 0; i < programLength; i++)
			prg[i] = program[i];
		for (int i = programLength; i < EEPROM_MEMORY; i++)
			prg[i] = 0xEA;

		WORD zeroAddress = (WORD)(MAX_MEMORY - EEPROM_MEMORY);
		prg[0xFFFC - zeroAddress] = zeroAddress & 0xFF;
		prg[0xFFFD - zeroAddress] = (zeroAddress >> 8);

		MCU->EEPROM.LoadProgram(prg);
		delete [] prg;
	}

	uint32_t RunTestProgram()
	{
		MCU->clock.Start();

		uint32_t cycles = -3;
		while (MCU->CPU.DataBus != 0xEA)
		{
			cycles++;
			MCU->RunCycle();
		}

		return cycles;
	}
};
