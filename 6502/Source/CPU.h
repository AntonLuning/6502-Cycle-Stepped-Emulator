#pragma once

#include "Base.h"
#include "Memory.h"

#include <functional>
#include <queue>

struct StatusFlags
{
	BYTE C  : 1;	// Carry (0)
	BYTE Z  : 1;	// Zero (1)
	BYTE I  : 1;	// Interrupt disable (2)
	BYTE D  : 1;	// Decimal (3)
	BYTE B  : 1;	// Break commnd (4)
	BYTE UB : 1;	// Unused (5)
	BYTE V  : 1;	// Overflow (6)
	BYTE N  : 1;	// Negative (7)

	void Reset()
	{
		C = Z = I = D = B = V = N = 0;
		UB = 1;
	}
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

private:
	BYTE m_PCL;
	WORD m_Calculated;
	std::queue<std::function<void()>> m_InstructionQueue;

	Memory* m_HandleSRAM = nullptr;
	Memory* m_HandleEEPROM = nullptr;

public:
	CPU();
	~CPU() = default;

	void Reset();
	void RunCycle(Memory* SRAM, Memory* EEPROM);

private:
	void SetDataBusFromMemory();
	void WriteMemoryFromDataBus();
	Memory* GetMemoryWithAddress(const WORD& address);

	void SetInstruction();

	void PushZeroPage();
	void PushZeroPageX();
	void PushZeroPageY();
	void PushAbsolute();
	void PushIndirectX();
	void PushIndirectY();

	void SetA();
	void StoreA();
	void SetX();
	void StoreX();
	void SetY();
	void StoreY();

	// Instructions
	void TAXImplied();
	void TAYImplied();
	void TSXImplied();
	void TXAImplied();
	void TXSImplied();
	void TYAImplied();
	
	void PHAImplied();
	void PHPImplied();
	void PLAImplied();
	void PLPImplied();

	void DECZeroPage();
	void DECZeroPageX();
	void DECAbsolute();
	void DECAbsoluteX();
	void INCZeroPage();
	void INCZeroPageX();
	void INCAbsolute();
	void INCAbsoluteX();

	void DEXImplied();
	void DEYImplied();
	void INXImplied();
	void INYImplied();

	void ADCImmediate();
	void ADCZeroPage();
	void ADCZeroPageX();
	void ADCAbsolute();
	void ADCAbsoluteX();
	void ADCAbsoluteY();
	void ADCIndirectX();
	void ADCIndirectY();
	void SBCImmediate();
	void SBCZeroPage();
	void SBCZeroPageX();
	void SBCAbsolute();
	void SBCAbsoluteX();
	void SBCAbsoluteY();
	void SBCIndirectX();
	void SBCIndirectY();

	void ANDImmediate();
	void ANDZeroPage();
	void ANDZeroPageX();
	void ANDAbsolute();
	void ANDAbsoluteX();
	void ANDAbsoluteY();
	void ANDIndirectX();
	void ANDIndirectY();
	void EORImmediate();
	void EORZeroPage();
	void EORZeroPageX();
	void EORAbsolute();
	void EORAbsoluteX();
	void EORAbsoluteY();
	void EORIndirectX();
	void EORIndirectY();
	void ORAImmediate();
	void ORAZeroPage();
	void ORAZeroPageX();
	void ORAAbsolute();
	void ORAAbsoluteX();
	void ORAAbsoluteY();
	void ORAIndirectX();
	void ORAIndirectY();

	void ASLAccumulator();
	void ASLZeroPage();
	void ASLZeroPageX();
	void ASLAbsolute();
	void ASLAbsoluteX();
	void LSRAccumulator();
	void LSRZeroPage();
	void LSRZeroPageX();
	void LSRAbsolute();
	void LSRAbsoluteX();
	void ROLAccumulator();
	void ROLZeroPage();
	void ROLZeroPageX();
	void ROLAbsolute();
	void ROLAbsoluteX();
	void RORAccumulator();
	void RORZeroPage();
	void RORZeroPageX();
	void RORAbsolute();
	void RORAbsoluteX();

	void BCCImplied();
	void BCSImplied();
	void BEQImplied();
	void BMIImplied();
	void BNEImplied();
	void BPLImplied();
	void BVCImplied();
	void BVSImplied();

	void CMPImmediate();
	void CMPZeroPage();
	void CMPZeroPageX();
	void CMPAbsolute();
	void CMPAbsoluteX();
	void CMPAbsoluteY();
	void CMPIndirectX();
	void CMPIndirectY();
	void CPXImmediate();
	void CPXZeroPage();
	void CPXAbsolute();
	void CPYImmediate();
	void CPYZeroPage();
	void CPYAbsolute();

	void BRKImplied();
	void JMPAbsolute();
	void JMPIndirect();
	void JSRAbsolute();
	void RTIImplied();
	void RTSImplied();

	void NOPImplied();
	void BITZeroPage();
	void BITAbsolute();
};