#pragma once

#include "Base.h"

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
	WORD AddressBus;
	BYTE DataBus;
	bool RWB;			// Read/Write bit (true if read)
	bool InternalCycle;

private:
	WORD PC;			// Program counter
	BYTE SP;			// Stack pointer
	StatusFlags PS;		// Program status
	BYTE A, X, Y;		// Registers (Accumulator, X/Y index)

	bool m_ResetCycle;
	BYTE m_ResetPCL;
	std::queue<std::function<void()>> m_InstructionQueue;

public:
	CPU() = default;
	~CPU() = default;

	void Reset();

	void StartCycle();
	void FinishCycle();
	void RunJob();

#ifndef DISTRIBUTION_6502
	inline WORD GetPC() { return PC; }
	inline BYTE GetSP() { return SP; }
	inline StatusFlags GetPS() { return PS; }
	inline BYTE GetA() { return A; }
	inline BYTE GetX() { return X; }
	inline BYTE GetY() { return Y; }
#endif // !DISTRIBUTION_6502

private:
	void SetInstruction();

	void PushEmpty();
	void PushSetAddressBusWithPC();
	void PushSetAddressBusWithDataBus();
	void PushSetA();

	// Instructions
	void LDAZeroPageX();
	void LDAAbsolute();
	void LDAAbsoluteX();
	void LDAAbsoluteY();
	void LDAIndirectX();
	void LDAIndirectY();
	void STAZeroPage();
	void STAZeroPageX();
	void STAAbsolute();
	void STAAbsoluteX();
	void STAAbsoluteY();
	void STAIndirectX();
	void STAIndirectY();

	void LDXImmediate();
	void LDXZeroPage();
	void LDXZeroPageY();
	void LDXAbsolute();
	void LDXAbsoluteY();
	void STXZeroPage();
	void STXZeroPageY();
	void STXAbsolute();

	void LDYImmediate();
	void LDYZeroPage();
	void LDYZeroPageX();
	void LDYAbsolute();
	void LDYAbsoluteX();
	void STYZeroPage();
	void STYZeroPageX();
	void STYAbsolute();
	
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

	void CLCImplied();
	void CLDImplied();
	void CLIImplied();
	void CLVImplied();
	void SECImplied();
	void SEDImplied();
	void SEIImplied();

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