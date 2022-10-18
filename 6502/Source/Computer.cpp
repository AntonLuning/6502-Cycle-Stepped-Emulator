#include "Computer.h"

#include "Log.h"

Computer::Computer(uint32_t sizeSRAM, uint32_t sizeEEPROM)
	: CPU(), clock()
{
	SRAM = new Memory(sizeSRAM, 0x0000, false);
	EEPROM = new Memory(sizeEEPROM, (WORD)(MAX_MEMORY - sizeEEPROM), true);

	LOG_INFO("SRAM initialized with address {0} -> {1} ({2} KB)", Log::WordToHexString((WORD)0), Log::WordToHexString((WORD)(sizeSRAM-1)), sizeSRAM / 1024);
	LOG_INFO("EEPROM initialized with address {0} -> {1} ({2} KB)", Log::WordToHexString((WORD)(MAX_MEMORY - sizeEEPROM)), Log::WordToHexString((WORD)(MAX_MEMORY-1)), sizeEEPROM / 1024);
	
	if (sizeSRAM + sizeEEPROM > MAX_MEMORY)
		LOG_WARN("SRAM and EEPROM have shared memory addresses. This may result in unwanted behaviour.");
}

void Computer::Reset()
{
	CPU.Reset();
	SRAM->Reset();
	EEPROM->Reset();
}

void Computer::RunCycle()
{
	if (!clock.IsRunning())
		return;
	clock.WaitForNextCycle();

	CPU.RunCycle(SRAM, EEPROM);
}
