#pragma once

#include "Base.h"
#include "Memory.h"

#include <functional>
#include <queue>

struct BitFlags
{
	BYTE C : 1;		// Carry (0)
	BYTE Z : 1;		// Zero (1)
	BYTE I : 1;		// Interrupt disable (2)
	BYTE D : 1;		// Decimal (3)
	BYTE B : 1;		// Break commnd (4)
	BYTE UB : 1;	// Unused (5)
	BYTE V : 1;		// Overflow (6)
	BYTE N : 1;		// Negative (7)
};

union StatusFlags
{
	BitFlags Bits;
	BYTE Byte;
};

class CPU
{
public:
	WORD PC;			// Program counter
	BYTE SP;			// Stack pointer
	StatusFlags PS;		// Program status
	BYTE A, X, Y;		// Registers (Accumulator, X/Y index)

	WORD AddressBus;
	BYTE DataBus;
	bool DataRead;		// Read/Write bit (RWB)
	bool NMI = false;
	bool IRQ = false;

private:
	friend class ComputerTest;

	BYTE m_BAL, m_BAH;
	BYTE m_ADL, m_ADH;
	BYTE m_IAL, m_IAH;
	std::queue<std::function<void()>> m_InstructionQueue;

	Memory* m_HandleSRAM = nullptr;
	Memory* m_HandleEEPROM = nullptr;

public:
	CPU();
	~CPU() = default;

	void Reset();
	void RunCycle(Memory* SRAM, Memory* EEPROM);

	void InterruptNMI();
	void InterruptIRQ();

private:
	void ClearInstructionQueue();

	void SetDataBusFromMemory();
	void WriteMemoryFromDataBus();
	Memory* GetMemoryWithAddress(const WORD& address);

	void LoadInstruction();

	void PushZeroPage();
	void PushZeroPageXY();
	void PushAbsolute();
	void PushAbsoluteXY();
	void PushIndirectX();
	void PushIndirectY();
	void PushZeroPageModified();
	void PushZeroPageXModified();
	void PushAbsoluteModified();
	void PushAbsoluteXModified();

	void SetRegister(BYTE& reg);
	void StoreRegister(BYTE& reg);
	void CompareRegister(BYTE& reg);
	void AddA();
	void SubA();
	void AndA();
	void OrA();
	void ExclusiveOrA();
	void ShiftLeftDB(bool withC);
	void ShiftRightDB(bool withC);
	void DecDB();
	void IncDB();
	void TestBit();
};