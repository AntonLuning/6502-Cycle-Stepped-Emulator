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
			AddressBus = BIT(8) | SP;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = BIT(8) | (SP - 1);
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = BIT(8) | (SP - 2);
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

#define EXTRA_CYCLE_CHECK(index, func)						\
	if (WORD(m_BAL) + index > 0xFF)							\
	{														\
		m_InstructionQueue.push([&]()						\
			{												\
				m_ADL = m_BAL + index;						\
				m_ADH = m_BAH + 1;							\
				AddressBus = ((WORD)m_ADH << 8) | m_ADL;	\
				##func;									\
			});												\
	}														\
	else													\
	{														\
		m_ADL = m_BAL + index;								\
		m_ADH = m_BAH;										\
		AddressBus = ((WORD)m_ADH << 8) | m_ADL;			\
		##func;											\
	}

#define BRANCH_CHECK(condition)										\
	if (condition)													\
	{																\
		m_InstructionQueue.push([&]()								\
			{														\
				if ((PC >> 8) != ((PC + (int8_t)DataBus) >> 8))		\
				{													\
					m_InstructionQueue.push([&]()					\
						{											\
							PC += (int8_t)DataBus;					\
							AddressBus = PC;						\
						});											\
				}													\
				else												\
				{													\
					PC += (int8_t)DataBus;							\
					AddressBus = PC;								\
				}													\
			});														\
	}

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
					SetRegister(A);
				});
		} break;
		case INS_LDA_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SetRegister(A);
				});
		} break;
		case INS_LDA_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					SetRegister(A);
				});
		} break;
		case INS_LDA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetRegister(A);
				});
		} break;
		case INS_LDA_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, SetRegister(A))
				});
		} break;
		case INS_LDA_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SetRegister(A))
				});
		} break;
		case INS_LDA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetRegister(A);
				});
		} break;
		case INS_LDA_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SetRegister(A))
				});
		} break;
		case INS_STA_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					StoreRegister(A);
				});
		} break;
		case INS_STA_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					StoreRegister(A);
				});
		} break;
		case INS_STA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreRegister(A);
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
					StoreRegister(A);
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
					StoreRegister(A);
				});
		} break;
		case INS_STA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreRegister(A);
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
					StoreRegister(A);
				});
		} break;

		case INS_LDX_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetRegister(X);
				});
		} break;
		case INS_LDX_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SetRegister(X);
				});
		} break;
		case INS_LDX_ZPY:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + Y);
					SetRegister(X);
				});
		} break;
		case INS_LDX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetRegister(X);
				});
		} break;
		case INS_LDX_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SetRegister(X))
				});
		} break;
		case INS_LDY_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetRegister(Y);
				});
		} break;
		case INS_LDY_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					SetRegister(Y);
				});
		} break;
		case INS_LDY_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					SetRegister(Y);
				});
		} break;
		case INS_LDY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					SetRegister(Y);
				});
		} break;
		case INS_LDY_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, SetRegister(Y))
				});
		} break;
		case INS_STX_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					StoreRegister(X);
				});
		} break;
		case INS_STX_ZPY:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + Y);
					StoreRegister(X);
				});
		} break;
		case INS_STX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreRegister(X);
				});
		} break;
		case INS_STY_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					StoreRegister(Y);
				});
		} break;
		case INS_STY_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					StoreRegister(Y);
				});
		} break;
		case INS_STY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					StoreRegister(Y);
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
#pragma endregion

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
					SetRegister(A);
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
#pragma endregion

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
#pragma endregion

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
					EXTRA_CYCLE_CHECK(X, AddA())
				});
		} break;
		case INS_ADC_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, AddA())
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
					EXTRA_CYCLE_CHECK(Y, AddA())
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
					EXTRA_CYCLE_CHECK(X, SubA())
				});
		} break;
		case INS_SBC_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, SubA())
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
					EXTRA_CYCLE_CHECK(Y, SubA())
				});
		} break;
#pragma endregion

#pragma region Logical_Instructions
		case INS_AND_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					AndA();
				});
		} break;
		case INS_AND_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					AndA();
				});
		} break;
		case INS_AND_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					AndA();
				});
		} break;
		case INS_AND_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					AndA();
				});
		} break;
		case INS_AND_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, AndA())
				});
		} break;
		case INS_AND_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, AndA())
				});
		} break;
		case INS_AND_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					AndA();
				});
		} break;
		case INS_AND_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, AndA())
				});
		} break;

		case INS_ORA_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					OrA();
				});
		} break;
		case INS_ORA_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					OrA();
				});
		} break;
		case INS_ORA_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					OrA();
				});
		} break;
		case INS_ORA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					OrA();
				});
		} break;
		case INS_ORA_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, OrA())
				});
		} break;
		case INS_ORA_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, OrA())
				});
		} break;
		case INS_ORA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					OrA();
				});
		} break;
		case INS_ORA_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, OrA())
				});
		} break;

		case INS_EOR_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					ExclusiveOrA();
				});
		} break;
		case INS_EOR_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					ExclusiveOrA();
				});
		} break;
		case INS_EOR_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					ExclusiveOrA();
				});
		} break;
		case INS_EOR_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					ExclusiveOrA();
				});
		} break;
		case INS_EOR_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, ExclusiveOrA())
				});
		} break;
		case INS_EOR_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, ExclusiveOrA())
				});
		} break;
		case INS_EOR_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					ExclusiveOrA();
				});
		} break;
		case INS_EOR_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, ExclusiveOrA())
				});
		} break;
	
		case INS_ASL_ACC:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					PS.Bits.C = (A & BIT(7)) > 0;
					A <<= 1;
					PS.Bits.Z = A == 0;
					PS.Bits.N = (A & BIT(7)) > 0;
				});
		} break;
		case INS_ASL_ZP:	// 5
		{
			PushZeroPageModified();
			m_InstructionQueue.push([&]()
				{
					ShiftLeftDB();
				});
		} break;
		case INS_ASL_ZPX:	// 6
		{
			PushZeroPageXModified();
			m_InstructionQueue.push([&]()
				{
					ShiftLeftDB();
				});
		} break;
		case INS_ASL_ABS:	// 6
		{
			PushAbsoluteModified();
			m_InstructionQueue.push([&]()
				{
					ShiftLeftDB();
				});
		} break;
		case INS_ASL_ABSX:	// 7
		{
			PushAbsoluteXModified();
			m_InstructionQueue.push([&]()
				{
					ShiftLeftDB();
				});
		} break;
		case INS_LSR_ACC:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					PS.Bits.C = (A & BIT(0)) > 0;
					A >>= 1;
					PS.Bits.Z = A == 0;
					PS.Bits.N = 0;
				});
		} break;
		case INS_LSR_ZP:	// 5
		{
			PushZeroPageModified();
			m_InstructionQueue.push([&]()
				{
					ShiftRightDB();
				});
		} break;
		case INS_LSR_ZPX:	// 6
		{
			PushZeroPageXModified();
			m_InstructionQueue.push([&]()
				{
					ShiftRightDB();
				});
		} break;
		case INS_LSR_ABS:	// 6
		{
			PushAbsoluteModified();
			m_InstructionQueue.push([&]()
				{
					ShiftRightDB();
				});
		} break;
		case INS_LSR_ABSX:	// 7
		{
			PushAbsoluteXModified();
			m_InstructionQueue.push([&]()
				{
					ShiftRightDB();
				});
		} break;
	
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
#pragma endregion

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
#pragma endregion

#pragma region Branch_Instructions
		case INS_BCC_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.C == 0);
				});
		} break;
		case INS_BCS_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.C == 1);
				});
		} break;
		case INS_BNE_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.Z == 0);
				});
		} break;
		case INS_BEQ_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.Z == 1);
				});
		} break;
		case INS_BPL_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.N == 0);
				});
		} break;
		case INS_BMI_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.N == 1);
				});
		} break;
		case INS_BVC_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.V == 0);
				});
		} break;
		case INS_BVS_REL:	// 2/3(4)
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					BRANCH_CHECK(PS.Bits.V == 1);
				});
		} break;

		case INS_CMP_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					CompareRegister(A);
				});
		} break;
		case INS_CMP_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					CompareRegister(A);
				});
		} break;
		case INS_CMP_ZPX:	// 4
		{
			PushZeroPageXY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = (BYTE)(m_BAL + X);
					CompareRegister(A);
				});
		} break;
		case INS_CMP_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					CompareRegister(A);
				});
		} break;
		case INS_CMP_ABSX:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(X, CompareRegister(A))
				});
		} break;
		case INS_CMP_ABSY:	// 4(5)
		{
			PushAbsoluteXY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, CompareRegister(A))
				});
		} break;
		case INS_CMP_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					CompareRegister(A);
				});
		} break;
		case INS_CMP_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					EXTRA_CYCLE_CHECK(Y, CompareRegister(A))
				});
		} break;

		case INS_CPX_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					CompareRegister(X);
				});
		} break;
		case INS_CPX_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					CompareRegister(X);
				});
		} break;
		case INS_CPX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					CompareRegister(X);
				});
		} break;
		case INS_CPY_IM:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					CompareRegister(Y);
				});
		} break;
		case INS_CPY_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					CompareRegister(Y);
				});
		} break;
		case INS_CPY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;
					CompareRegister(Y);
				});
		} break;
#pragma endregion

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
		case INS_JMP_ABS:	// 3
		{
			m_InstructionQueue.push([&]()
				{
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
		case INS_JMP_IND:	// 5
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					m_IAL = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					m_IAH = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_IAH << 8) | m_IAL;
					SetDataBusFromMemory();
					m_ADL = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = (((WORD)m_IAH << 8) | m_IAL) + 1;
					SetDataBusFromMemory();
					m_ADH = DataBus;
					PC = ((WORD)m_ADH << 8) | m_ADL;
				});
		} break;
		case INS_JSR_ABS:	// 6
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
					m_ADL = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP;
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
					DataBus = PC;
					WriteMemoryFromDataBus();
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
					m_ADH = DataBus;
					PC = ((WORD)m_ADH << 8) | m_ADL;
				});
		} break;
		case INS_RTS_IMP:	// 6
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
					PC = DataBus;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = BIT(8) | SP;
					SetDataBusFromMemory();
					PC |= (WORD)DataBus << 8;
				});
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC++;
					SetDataBusFromMemory();
				});
		} break;
#pragma endregion

#pragma region Other_Instructions
		case INS_NOP_IMP:	// 2
		{
			m_InstructionQueue.push([&]()
				{
					AddressBus = PC;
					SetDataBusFromMemory();
				});
		} break;
		case INS_BIT_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_ADL;
					TestBit();	
				});
		} break;
		case INS_BIT_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)m_ADH << 8) | m_ADL;				
					TestBit();
				});
		} break;
#pragma endregion

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

void CPU::SetRegister(BYTE& reg)
{
	SetDataBusFromMemory();
	reg = DataBus;
	PS.Bits.Z = reg == 0;
	PS.Bits.N = (reg & BIT(7)) > 0;
}

void CPU::StoreRegister(BYTE& reg)
{
	DataBus = reg;
	WriteMemoryFromDataBus();
}

void CPU::CompareRegister(BYTE& reg)
{
	SetDataBusFromMemory();
	int16_t result = reg - DataBus;

	PS.Bits.C = (result & BIT(8)) > 0;
	PS.Bits.Z = BYTE(result) == 0;
	PS.Bits.N = (result & BIT(7)) > 0;
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

void CPU::AndA()
{
	SetDataBusFromMemory();
	A &= DataBus;
	PS.Bits.Z = A == 0;
	PS.Bits.N = (A & BIT(7)) > 0;
}

void CPU::OrA()
{
	SetDataBusFromMemory();
	A |= DataBus;
	PS.Bits.Z = A == 0;
	PS.Bits.N = (A & BIT(7)) > 0;
}

void CPU::ExclusiveOrA()
{
	SetDataBusFromMemory();
	A ^= DataBus;
	PS.Bits.Z = A == 0;
	PS.Bits.N = (A & BIT(7)) > 0;
}

void CPU::ShiftLeftDB()
{
	PS.Bits.C = (DataBus & BIT(7)) > 0;
	DataBus <<= 1;
	WriteMemoryFromDataBus();
	PS.Bits.Z = DataBus == 0;
	PS.Bits.N = (DataBus & BIT(7)) > 0;
}

void CPU::ShiftRightDB()
{
	PS.Bits.C = (DataBus & BIT(0)) > 0;
	DataBus >>= 1;
	WriteMemoryFromDataBus();
	PS.Bits.Z = DataBus == 0;
	PS.Bits.N = 0;
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

void CPU::TestBit()
{
	SetDataBusFromMemory();
	PS.Bits.Z = (A & DataBus) == 0;
	PS.Bits.V = (DataBus & BIT(6)) > 0;
	PS.Bits.N = (DataBus & BIT(7)) > 0;
}
