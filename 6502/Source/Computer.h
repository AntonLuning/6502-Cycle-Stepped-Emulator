#pragma once

#include "Base.h"
#include "CPU.h"
#include "Clock.h"
#include "Memory.h"

constexpr uint32_t MAX_MEMORY = 64 * 1024;

// Little endian (least-significant byte at the smallest address)
// 0x0200 -> 0xFFFA	: General purpose or I/O
// 
// --- SRAM -----------------------------------------------------------------
// 0x0000 -> 0x00FF (256 bytes): Zero Page (reserved for faster instructions)
// 0x0100 -> 0x01FF (256 bytes): Systems stack
// 
// --- EEPROM ---------------------------------------------------------------
// 0xFFFA -> 0xFFFF (6 bytes)  : Must be programemd with the following:
//    0xFFFA/B : Non-Maskable Interrupt (NMI)
//    0xFFFC/D : Reset (RES)
//    0xFFFE/F : Interrupt Request (IRQ)

class Computer
{
public:
	CPU CPU;
	Clock clock;
	Memory SRAM;
	Memory EEPROM;

	// TODO: I/O
	// Display display;

public:
	Computer(uint32_t sizeSRAM, uint32_t sizeEEPROM);
	~Computer() = default;

	void Reset();
	void RunCycle();
};
