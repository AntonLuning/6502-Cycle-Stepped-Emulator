#include "CPU.h"

#include "Log.h"
#include "OperationCodes.h"

CPU::CPU()
{
	Reset();
}

void CPU::Reset()
{
	ClearInstructionQueue();

	m_InstructionQueue.push([&]()
		{
			PS.Bits.I = 1;
			SP = 0x00;
		});
	m_InstructionQueue.push([&]() {});
	m_InstructionQueue.push([&]() {});
	m_InstructionQueue.push([&]()
		{
			AddressBus = 0x0100;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = 0x01FF;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = 0x01FE;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			SP = 0xFD;
			PC = 0xFFFC;
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC;
			SetDataBusFromMemory();
			m_ADH = DataBus;
			PC = ((WORD)m_ADH << 8) | m_ADL;
		});
}

void CPU::RunCycle(Memory* SRAM, Memory* EEPROM)
{
	m_HandleSRAM = SRAM;
	m_HandleEEPROM = EEPROM;

	if (!m_InstructionQueue.empty())
	{
		m_InstructionQueue.front()();
		m_InstructionQueue.pop();
	}
	else
		LoadInstruction();

	PRINT_CPU("{0} - {1} {2}", Log::WordToHexString(AddressBus), DataRead ? "\"r\"" : "\"W\"", Log::WordToHexString(DataBus));
}

void CPU::InterruptNMI()
{
	NMI = true;
}

void CPU::InterruptIRQ()
{
	if (PS.Bits.I == 1)
		return;

	IRQ = true;
}

void CPU::ClearInstructionQueue()
{
	std::queue<std::function<void()>> emptyQueue;
	std::swap(m_InstructionQueue, emptyQueue);
}

void CPU::SetDataBusFromMemory()
{
	Memory* activeMemory = GetMemoryWithAddress(AddressBus);
	if (activeMemory != nullptr)
	{
		DataBus = activeMemory->ReadByte(AddressBus);
		DataRead = true;
	}
}

void CPU::WriteMemoryFromDataBus()
{
	Memory* activeMemory = GetMemoryWithAddress(AddressBus);
	if (activeMemory != nullptr)
	{
		if (activeMemory->IsROM())
			LOG_ERROR("Writing to read-only memory (address {0})", Log::WordToHexString(AddressBus));

		activeMemory->WriteByte(AddressBus, DataBus);
		DataRead = false;
	}
}

Memory* CPU::GetMemoryWithAddress(const WORD& address)
{
	if (m_HandleSRAM->IsAddressOk(address))
		return m_HandleSRAM;
	else if (m_HandleEEPROM->IsAddressOk(address))
		return m_HandleEEPROM;

	LOG_ERROR("Address {0} is outside the accessible SRAM, EEPROM, and I/O!", Log::WordToHexString(address));
	return nullptr;
}

#define EXTRA_CYCLE_CHECK(index, func)	if (WORD(m_BAL) + index > 0xFF) \
										{ \
											m_InstructionQueue.push([&]() \
												{ \
													m_ADL = m_BAL + index; \
													m_ADH = m_BAH + 1; \
													AddressBus = ((WORD)m_ADH << 8) | m_ADL; \
													##func(); \
												}); \
										} \
										else \
										{ \
											m_ADL = m_BAL + index; \
											m_ADH = m_BAH; \
											AddressBus = ((WORD)m_ADH << 8) | m_ADL; \
											##func(); \
										} \

void CPU::LoadInstruction()
{
	if (NMI || IRQ)
		AddressBus = 0x00;
	else
		AddressBus = PC++;
	
	SetDataBusFromMemory();

	switch (DataBus)
	{
#pragma region Transfer_Instructions
		case INS_LDA_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetA();
				});
		} break;
		case INS_LDA_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SetA();
				});
		} break;
		case INS_LDA_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					SetA();
				});
		} break;
		case INS_LDA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetA();
				});
		} break;
		case INS_LDA_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, SetA)
				});
		} break;
		case INS_LDA_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SetA)
				});
		} break;
		case INS_LDA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetA();
				});
		} break;
		case INS_LDA_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SetA)
				});
		} break;
		case INS_STA_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					StoreA();
				});
		} break;
		case INS_STA_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					StoreA();
				});
		} break;
		case INS_STA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreA();
				});
		} break;
		case INS_STA_ABSX:	// 5
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					m_ADL = m_BAL + X;
					m_ADH = m_BAH + (WORD(m_BAL) + X > 0xFF ? 1 : 0);
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
				});
			m_InstructionQueue.push([&]()
				{
					StoreA();
				});
		} break;
		case INS_STA_ABSY:	// 5
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					m_ADL = m_BAL + Y;
					m_ADH = m_BAH + (WORD(m_BAL) + Y > 0xFF ? 1 : 0);
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
				});
			m_InstructionQueue.push([&]()
				{
					StoreA();
				});
		} break;
		case INS_STA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreA();
				});
		} break;
		case INS_STA_INDY:	// 6
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					m_ADL = m_BAL + Y;
					m_ADH = m_BAH + (WORD(m_BAL) + Y > 0xFF ? 1 : 0);
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
				});
			m_InstructionQueue.push([&]()
				{
					StoreA();
				});
		} break;

		case INS_LDX_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetX();
				});
		} break;
		case INS_LDX_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SetX();
				});
		} break;
		case INS_LDX_ZPY:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + Y);
					SetX();
				});
		} break;
		case INS_LDX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetX();
				});
		} break;
		case INS_LDX_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SetX)
				});
		} break;
		case INS_LDY_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetY();
				});
		} break;
		case INS_LDY_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SetY();
				});
		} break;
		case INS_LDY_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					SetY();
				});
		} break;
		case INS_LDY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetY();
				});
		} break;
		case INS_LDY_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, SetY)
				});
		} break;
		case INS_STX_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					StoreX();
				});
		} break;
		case INS_STX_ZPY:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + Y);
					StoreX();
				});
		} break;
		case INS_STX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreX();
				});
		} break;
		case INS_STY_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					StoreY();
				});
		} break;
		case INS_STY_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					StoreY();
				});
		} break;
		case INS_STY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreY();
				});
		} break;

		case INS_TAX_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					X = A;
					PS.Bits.Z = X == 0;
					PS.Bits.N = (X & BIT(7)) > 0;
				});
		} break;
		case INS_TAY_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					Y = A;
					PS.Bits.Z = Y == 0;
					PS.Bits.N = (Y & BIT(7)) > 0;
				});
		} break;
		case INS_TSX_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					X = SP;
					PS.Bits.Z = X == 0;
					PS.Bits.N = (X & BIT(7)) > 0;
				});
		} break;
		case INS_TXA_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					A = X;
					PS.Bits.Z = A == 0;
					PS.Bits.N = (A & BIT(7)) > 0;
				});
		} break;
		case INS_TXS_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					SP = X;
				});
		} break;
		case INS_TYA_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					A = Y;
					PS.Bits.Z = A == 0;
					PS.Bits.N = (A & BIT(7)) > 0;
				});
		} break;
#pragma endregion Transfer_Instructions

#pragma region PushPull_Instructions
		case INS_PHA_IMP:	// 3
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP--;
					DataBus = A;
					WriteMemoryFromDataBus();
				});
		} break;
		case INS_PHP_IMP:	// 3
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP--;
					DataBus = PS.Byte;
					WriteMemoryFromDataBus();
				});
		} break;
		case INS_PLA_IMP:	// 4
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP++;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP;
					SetA();
				});
		} break;
		case INS_PLP_IMP:	// 4
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP++;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP;
					SetDataBusFromMemory();
					PS.Byte = DataBus;
				});
		} break;
#pragma endregion PushPull_Instructions

#pragma region DecInc_Instructions
		case INS_DEC_ZP:	// 5
		{
			PushZeroPageModified();
			m_InstructionQueue.push([&]()
				{
					DecDB();
				});
		} break;
		case INS_DEC_ZPX:	// 6
		{
			PushZeroPageXModified();
			m_InstructionQueue.push([&]()
				{
					DecDB();
				});
		} break;
		case INS_DEC_ABS:	// 6
		{
			PushAbsoluteModified();
			m_InstructionQueue.push([&]()
				{
					DecDB();
				});
		} break;
		case INS_DEC_ABSX:	// 7
		{
			PushAbsoluteXModified();
			m_InstructionQueue.push([&]()
				{
					DecDB();
				});
		} break;
		case INS_INC_ZP:	// 5
		{
			PushZeroPageModified();
			m_InstructionQueue.push([&]()
				{
					IncDB();
				});
		} break;
		case INS_INC_ZPX:	// 6
		{
			PushZeroPageXModified();
			m_InstructionQueue.push([&]()
				{
					IncDB();
				});
		} break;
		case INS_INC_ABS:	// 6
		{
			PushAbsoluteModified();
			m_InstructionQueue.push([&]()
				{
					IncDB();
				});
		} break;
		case INS_INC_ABSX:	// 7
		{
			PushAbsoluteXModified();
			m_InstructionQueue.push([&]()
				{
					IncDB();
				});
		} break;

		case INS_DEX_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					X--;
					PS.Bits.Z = X == 0;
					PS.Bits.N = (X & BIT(7)) > 0;
				});
		} break;
		case INS_INX_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					X++;
					PS.Bits.Z = X == 0;
					PS.Bits.N = (X & BIT(7)) > 0;
				});
		} break;
		case INS_DEY_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					Y--;
					PS.Bits.Z = Y == 0;
					PS.Bits.N = (Y & BIT(7)) > 0;
				});
		} break;
		case INS_INY_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					Y++;
					PS.Bits.Z = Y == 0;
					PS.Bits.N = (Y & BIT(7)) > 0;
				});
		} break;
#pragma endregion DecInc_Instructions

#pragma region Arithmetic_Instructions
		case INS_ADC_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					AddA();
				});
		} break;
		case INS_ADC_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					AddA();
				});
		} break;
		case INS_ADC_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					AddA();
				});
		} break;
		case INS_ADC_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					AddA();
				});
		} break;
		case INS_ADC_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, AddA)
				});
		} break;
		case INS_ADC_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, AddA)
				});
		} break;
		case INS_ADC_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					AddA();
				});
		} break;
		case INS_ADC_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, AddA)
				});
		} break;

		case INS_SBC_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SubA();
				});
		} break;
		case INS_SBC_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SubA();
				});
		} break;
		case INS_SBC_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					SubA();
				});
		} break;
		case INS_SBC_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SubA();
				});
		} break;
		case INS_SBC_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, SubA)
				});
		} break;
		case INS_SBC_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SubA)
				});
		} break;
		case INS_SBC_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SubA();
				});
		} break;
		case INS_SBC_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SubA)
				});
		} break;
#pragma endregion Arithmetic_Instructions

#pragma region Logical_Instructions
		//SWITCH_INS(INS_AND_IM, ANDImmediate)  	// Immediate
		//SWITCH_INS(INS_AND_ZP, ANDZeroPage)		// Zero Page
		//SWITCH_INS(INS_AND_ZPX, ANDZeroPageX)	// Zero Page X
		//SWITCH_INS(INS_AND_ABS, ANDAbsolute)	// Absolute
		//SWITCH_INS(INS_AND_ABSX, ANDAbsoluteX)	// Absolute X
		//SWITCH_INS(INS_AND_ABSY, ANDAbsoluteY)	// Absolute Y
		//SWITCH_INS(INS_AND_INDX, ANDIndirectX)	// Indirect X
		//SWITCH_INS(INS_AND_INDY, ANDIndirectY)	// Indirect Y
		//SWITCH_INS(INS_ASL_ACC, ASLAccumulator)	// Accumulator
		//SWITCH_INS(INS_ASL_ZP, ASLZeroPage)		// Zero Page
		//SWITCH_INS(INS_ASL_ZPX, ASLZeroPageX) 	// Zero Page X
		//SWITCH_INS(INS_ASL_ABS, ASLAbsolute)  	// Absolute
		//SWITCH_INS(INS_ASL_ABSX, ASLAbsoluteX)	// Absolute X
		//SWITCH_INS(INS_EOR_IM, EORImmediate)  	// Immediate
		//SWITCH_INS(INS_EOR_ZP, EORZeroPage)		// Zero Page
		//SWITCH_INS(INS_EOR_ZPX, EORZeroPageX)	// Zero Page X
		//SWITCH_INS(INS_EOR_ABS, EORAbsolute)	// Absolute
		//SWITCH_INS(INS_EOR_ABSX, EORAbsoluteX)	// Absolute X
		//SWITCH_INS(INS_EOR_ABSY, EORAbsoluteY)	// Absolute Y
		//SWITCH_INS(INS_EOR_INDX, EORIndirectX)	// Indirect X
		//SWITCH_INS(INS_EOR_INDY, EORIndirectY)	// Indirect Y
		//SWITCH_INS(INS_LSR_ACC, LSRAccumulator)	// Accumulator
		//SWITCH_INS(INS_LSR_ZP, LSRZeroPage)		// Zero Page
		//SWITCH_INS(INS_LSR_ZPX, LSRZeroPageX)	// Zero Page X
		//SWITCH_INS(INS_LSR_ABS, LSRAbsolute)	// Absolute
		//SWITCH_INS(INS_LSR_ABSX, LSRAbsoluteX)	// Absolute X
		//SWITCH_INS(INS_ORA_IM, ORAImmediate)	// Immediate
		//SWITCH_INS(INS_ORA_ZP, ORAZeroPage)		// Zero Page
		//SWITCH_INS(INS_ORA_ZPX, ORAZeroPageX)  	// Zero Page X
		//SWITCH_INS(INS_ORA_ABS, ORAAbsolute)   	// Absolute
		//SWITCH_INS(INS_ORA_ABSX, ORAAbsoluteX) 	// Absolute X
		//SWITCH_INS(INS_ORA_ABSY, ORAAbsoluteY) 	// Absolute Y
		//SWITCH_INS(INS_ORA_INDX, ORAIndirectX) 	// Indirect X
		//SWITCH_INS(INS_ORA_INDY, ORAIndirectY) 	// Indirect Y
		//SWITCH_INS(INS_ROL_ACC, ROLAccumulator)	// Accumulator
		//SWITCH_INS(INS_ROL_ZP, ROLZeroPage)		// Zero Page
		//SWITCH_INS(INS_ROL_ZPX, ROLZeroPageX)  	// Zero Page X
		//SWITCH_INS(INS_ROL_ABS, ROLAbsolute)   	// Absolute
		//SWITCH_INS(INS_ROL_ABSX, ROLAbsoluteX) 	// Absolute X
		//SWITCH_INS(INS_ROR_ACC, RORAccumulator)	// Accumulator
		//SWITCH_INS(INS_ROR_ZP, RORZeroPage)		// Zero Page
		//SWITCH_INS(INS_ROR_ZPX, RORZeroPageX) 	// Zero Page X
		//SWITCH_INS(INS_ROR_ABS, RORAbsolute)  	// Absolute
		//SWITCH_INS(INS_ROR_ABSX, RORAbsoluteX)	// Absolute X
#pragma endregion Logical_Instructions

#pragma region Flag_Instructions
		case INS_CLC_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.C = 0;
				});
		} break;
		case INS_CLD_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.D = 0;
				});
		} break;
		case INS_CLI_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.I = 0;
				});
		} break;
		case INS_CLV_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.V = 0;
				});
		} break;
		case INS_SEC_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.C = 1;
				});
		} break;
		case INS_SED_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.D = 1;
				});
		} break;
		case INS_SEI_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					PS.Bits.I = 1;
				});
		} break;
#pragma endregion Flag_Instructions

#pragma region Branch_Instructions
		//SWITCH_INS(INS_BCC_REL, BCCImplied)		// Relative
		//SWITCH_INS(INS_BCS_REL, BCSImplied)		// Relative
		//SWITCH_INS(INS_BEQ_REL, BEQImplied)		// Relative
		//SWITCH_INS(INS_BMI_REL, BMIImplied)		// Relative
		//SWITCH_INS(INS_BNE_REL, BNEImplied)		// Relative
		//SWITCH_INS(INS_BPL_REL, BPLImplied)		// Relative
		//SWITCH_INS(INS_BVC_REL, BVCImplied)		// Relative
		//SWITCH_INS(INS_BVS_REL, BVSImplied)		// Relative
		//SWITCH_INS(INS_CMP_IM, CMPImmediate)	// Immediate
		//SWITCH_INS(INS_CMP_ZP, CMPZeroPage)		// Zero Page
		//SWITCH_INS(INS_CMP_ZPX, CMPZeroPageX) 	// Zero Page X
		//SWITCH_INS(INS_CMP_ABS, CMPAbsolute)  	// Absolute
		//SWITCH_INS(INS_CMP_ABSX, CMPAbsoluteX)	// Absolute X
		//SWITCH_INS(INS_CMP_ABSY, CMPAbsoluteY)	// Absolute Y
		//SWITCH_INS(INS_CMP_INDX, CMPIndirectX)	// Indirect X
		//SWITCH_INS(INS_CMP_INDY, CMPIndirectY)	// Indirect Y
		//SWITCH_INS(INS_CPX_IM, CPXImmediate)  	// Immediate
		//SWITCH_INS(INS_CPX_ZP, CPXZeroPage)		// Zero Page
		//SWITCH_INS(INS_CPX_ABS, CPXAbsolute)	// Absolute
		//SWITCH_INS(INS_CPY_IM, CPYImmediate)	// Immediate
		//SWITCH_INS(INS_CPY_ZP, CPYZeroPage)		// Zero Page
		//SWITCH_INS(INS_CPY_ABS, CPYAbsolute)	// Absolute
#pragma endregion Branch_Instructions

#pragma region Jump_Instructions
		case INS_BRK_IMP:	// 7
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					if (!(NMI || IRQ))
						PC++;

					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP--;
					DataBus = PC >> 8;
					WriteMemoryFromDataBus();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP--;
					DataBus = (BYTE)PC;
					WriteMemoryFromDataBus();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP--;
					DataBus = PS.Byte;
					WriteMemoryFromDataBus();
				});
			m_InstructionQueue.push([&]()
				{
					PS.Bits.I = 1;
					if (NMI || IRQ)
						PS.Bits.B = 0;
					else
						PS.Bits.B = 1;
					
					if (NMI)
						PC = 0xFFFA;
					else
						PC = 0xFFFE;

					IRQ = false;
					NMI = false;

					AddressBus = PC++;
					SetDataBusFromMemory();
					m_ADL = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					m_ADH = DataBus;
					PC = ((WORD)m_ADH << 8) | m_ADL;
				});
		} break;
		case INS_RTI_IMP:	// 6
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP++;
					SetDataBusFromMemory();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP++;
					SetDataBusFromMemory();
					PS.Byte = DataBus;
					PS.Bits.B = 0;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP++;
					SetDataBusFromMemory();
					PC = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP;
					SetDataBusFromMemory();
					PC |= (WORD)DataBus << 8;
				});
		} break;
		//SWITCH_INS(INS_JMP_ABS, JMPAbsolute)	// Absolute
		//SWITCH_INS(INS_JMP_IND, JMPIndirect)	// Indirect
		//SWITCH_INS(INS_JSR_ABS, JSRAbsolute)	// 6
		//SWITCH_INS(INS_RTS_IMP, RTSImplied)		// Implied
#pragma endregion Jump_Instructions

#pragma region Other_Instructions
		case INS_NOP_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
		} break;
		//SWITCH_INS(INS_BIT_ZP, BITZeroPage)		// Zero Page
		//SWITCH_INS(INS_BIT_ABS, BITAbsolute)	// Absolute
#pragma endregion Other_Instructions

		default:
			LOG_ERROR("Illegal Opcode ({0}), instruction not handled!", Log::WordToHexString(DataBus));
			break;
	}
}

void CPU::PushZeroPage()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADL = DataBus;
		});
}

void CPU::PushZeroPageXY()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_BAL;
		});
}

void CPU::PushAbsolute()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADH = DataBus;
		});
}

void CPU::PushAbsoluteXY()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAH = DataBus;
		});
}

void CPU::PushIndirectX()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_BAL;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = (BYTE)(m_BAL + X);
			SetDataBusFromMemory();
			m_ADL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = (BYTE)(m_BAL + X) + 1;
			SetDataBusFromMemory();
			m_ADH = DataBus;
		});
}

void CPU::PushIndirectY()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_IAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_IAL;
			SetDataBusFromMemory();
			m_BAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_IAL + 1;
			SetDataBusFromMemory();
			m_BAH = DataBus;
		});
}

void CPU::PushZeroPageModified()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_ADL;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			DataRead = false;
		});
}

void CPU::PushZeroPageXModified()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_BAL;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = (BYTE)(m_BAL + X);
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			DataRead = false;
		});
}

void CPU::PushAbsoluteModified()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_ADH = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = ((WORD)m_ADH << 8) | m_ADL;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			DataRead = false;
		});
}

void CPU::PushAbsoluteXModified()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_BAH = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			m_ADL = m_BAL + X;
			m_ADH = m_BAH + (WORD(m_BAL) + X > 0xFF ? 1 : 0);
			AddressBus = ((WORD)m_ADH << 8) | m_ADL;
		});
	m_InstructionQueue.push([&]()
		{
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			DataRead = false;
		});
}

void CPU::SetA()
{
	SetDataBusFromMemory();
	A = DataBus;
	PS.Bits.Z = A == 0;
	PS.Bits.N = (A & BIT(7)) > 0;
}

void CPU::StoreA()
{
	DataBus = A;
	WriteMemoryFromDataBus();
}

void CPU::SetX()
{
	SetDataBusFromMemory();
	X = DataBus;
	PS.Bits.Z = X == 0;
	PS.Bits.N = (X & BIT(7)) > 0;
}

void CPU::StoreX()
{
	DataBus = X;
	WriteMemoryFromDataBus();
}

void CPU::SetY()
{
	SetDataBusFromMemory();
	Y = DataBus;
	PS.Bits.Z = Y == 0;
	PS.Bits.N = (Y & BIT(7)) > 0;
}

void CPU::StoreY()
{
	DataBus = Y;
	WriteMemoryFromDataBus();
}

void CPU::AddA()
{
	// TODO: Handle if decimal flag is set
	SetDataBusFromMemory();
	int16_t result = A + PS.Bits.C + DataBus;

	PS.Bits.V = ((A ^ result) & (DataBus ^ result) & BIT(7)) > 0;
	A = (result & 0xFF);
	PS.Bits.C = (result & 0xFF00) > 0;
	PS.Bits.Z = A == 0;
	PS.Bits.N = (A & BIT(7)) > 0;
}

void CPU::SubA()
{
	// TODO: Handle if decimal flag is set
	SetDataBusFromMemory();
	int16_t result = (A | (PS.Bits.C & BIT(8))) - DataBus;
	
	PS.Bits.V = ((A ^ result) & ((0xFF - DataBus) ^ result) & BIT(7)) > 0;
	A = (result & 0xFF);
	PS.Bits.C = (result & BIT(8)) > 0;
	PS.Bits.Z = A == 0;
	PS.Bits.N = (A & BIT(7)) > 0;
}

void CPU::DecDB()
{
	DataBus--;
	WriteMemoryFromDataBus();
	PS.Bits.Z = DataBus == 0;
	PS.Bits.N = (DataBus & BIT(7)) > 0;
}

void CPU::IncDB()
{
	DataBus++;
	WriteMemoryFromDataBus();
	PS.Bits.Z = DataBus == 0;
	PS.Bits.N = (DataBus & BIT(7)) > 0;
}
