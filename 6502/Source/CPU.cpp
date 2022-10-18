#include "CPU.h"

#include "Log.h"
#include "OperationCodes.h"

CPU::CPU()
{
	Reset();
}

void CPU::Reset()
{
	std::queue<std::function<void()>> emptyQueue;
	std::swap(m_InstructionQueue, emptyQueue);

	m_InstructionQueue.push([&]()
		{
			PS.I = 1;
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
			m_PCL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			PC = ((WORD)DataBus << 8) | m_PCL;
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
	{
		AddressBus = PC++;	
		SetDataBusFromMemory();
		SetInstruction();
	}

	PRINT_CPU("{0} - {1} {2}", Log::WordToHexString(AddressBus), DataRead ? "\"r\"" : "\"W\"", Log::WordToHexString(DataBus));
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


#define SWITCH_INS(ins, fn)	case ins: \
								fn(); \
								break;

void CPU::SetInstruction()
{
	switch (DataBus)
	{
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
					AddressBus = DataBus;
					SetA();
				});
		} break;
		case INS_LDA_ZPX:	// 4
		{
			PushZeroPageX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_Calculated;
					SetA();
				});
		} break;
		case INS_LDA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					SetA();
				});
		} break;
		case INS_LDA_ABSX:	// 4(5)
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + X > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + X;
								SetA();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + X;
						SetA();
					}
				});
		} break;
		case INS_LDA_ABSY:	// 4(5)
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + Y > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
								SetA();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
						SetA();
					}
				});
		} break;
		case INS_LDA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					SetA();
				});
		} break;
		case INS_LDA_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + Y > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
								SetA();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
						SetA();
					}
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
					AddressBus = DataBus;
					SetX();
				});
		} break;
		case INS_LDX_ZPY:	// 4
		{
			PushZeroPageY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_Calculated;
					SetX();
				});
		} break;
		case INS_LDX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					SetX();
				});
		} break;
		case INS_LDX_ABSY:	// 4(5)
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + Y > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
								SetX();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
						SetX();
					}
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
					AddressBus = DataBus;
					SetY();
				});
		} break;
		case INS_LDY_ZPX:	// 4
		{
			PushZeroPageX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_Calculated;
					SetY();
				});
		} break;
		case INS_LDY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					SetY();
				});
		} break;
		case INS_LDY_ABSX:	// 4(5)
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + X > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + X;
								SetY();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + X;
						SetY();
					}
				});
		} break;
		case INS_STA_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = DataBus;
					StoreA();
				});
		} break;
		case INS_STA_ZPX:	// 4
		{
			PushZeroPageX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_Calculated;
					StoreA();
				});
		} break;
		case INS_STA_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					StoreA();
				});
		} break;
		case INS_STA_ABSX:	// 4(5)
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + X > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + X;
								StoreA();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + X;
						StoreA();
					}
				});
		} break;
		case INS_STA_ABSY:	// 4(5)
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + Y > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
								StoreA();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
						StoreA();
					}
				});
		} break;
		case INS_STA_INDX:	// 6
		{
			PushIndirectX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					StoreA();
				});
		} break;
		case INS_STA_INDY:	// 5(6)
		{
			PushIndirectY();
			m_InstructionQueue.push([&]()
				{
					if (WORD(m_PCL) + Y > 0xFF)
					{
						m_InstructionQueue.push([&]()
							{
								AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
								StoreA();
							});
					}
					else
					{
						AddressBus = (((WORD)DataBus << 8) | m_PCL) + Y;
						StoreA();
					}
				});
		} break;	
		case INS_STX_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = DataBus;
					StoreX();
				});
		} break;
		case INS_STX_ZPY:	// 4
		{
			PushZeroPageY();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_Calculated;
					StoreX();
				});
		} break;
		case INS_STX_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					StoreX();
				});
		} break;
		case INS_STY_ZP:	// 3
		{
			PushZeroPage();
			m_InstructionQueue.push([&]()
				{
					AddressBus = DataBus;
					StoreY();
				});
		} break;
		case INS_STY_ZPX:	// 4
		{
			PushZeroPageX();
			m_InstructionQueue.push([&]()
				{
					AddressBus = m_Calculated;
					StoreY();
				});
		} break;
		case INS_STY_ABS:	// 4
		{
			PushAbsolute();
			m_InstructionQueue.push([&]()
				{
					AddressBus = ((WORD)DataBus << 8) | m_PCL;
					StoreY();
				});
		} break;

		SWITCH_INS(INS_TAX_IMP, TAXImplied)		// Implied
		SWITCH_INS(INS_TAY_IMP, TAYImplied)		// Implied
		SWITCH_INS(INS_TSX_IMP, TSXImplied)		// Implied
		SWITCH_INS(INS_TXA_IMP, TXAImplied)		// Implied
		SWITCH_INS(INS_TXS_IMP, TXSImplied)		// Implied
		SWITCH_INS(INS_TYA_IMP, TYAImplied)		// Implied
		SWITCH_INS(INS_PHA_IMP, PHAImplied)		// Implied
		SWITCH_INS(INS_PHP_IMP, PHPImplied)		// Implied
		SWITCH_INS(INS_PLA_IMP, PLAImplied)		// Implied
		SWITCH_INS(INS_PLP_IMP, PLPImplied)		// Implied
		SWITCH_INS(INS_DEC_ZP, DECZeroPage)		// Zero Page
		SWITCH_INS(INS_DEC_ZPX, DECZeroPageX)	// Zero Page X
		SWITCH_INS(INS_DEC_ABS, DECAbsolute)	// Absolute
		SWITCH_INS(INS_DEC_ABSX, DECAbsoluteX)	// Absolute X
		SWITCH_INS(INS_DEX_IMP, DEXImplied)		// Implied
		SWITCH_INS(INS_DEY_IMP, DEYImplied)		// Implied
		SWITCH_INS(INS_INC_ZP, INCZeroPage)		// Zero Page
		SWITCH_INS(INS_INC_ZPX, INCZeroPageX)	// Zero Page X
		SWITCH_INS(INS_INC_ABS, INCAbsolute)	// Absolute
		SWITCH_INS(INS_INC_ABSX, INCAbsoluteX)	// Absolute X
		SWITCH_INS(INS_INX_IMP, INXImplied)		// Implied
		SWITCH_INS(INS_INY_IMP, INYImplied)		// Implied

		SWITCH_INS(INS_ADC_IM, ADCImmediate)	// Immediate
		SWITCH_INS(INS_ADC_ZP, ADCZeroPage)		// Zero Page
		SWITCH_INS(INS_ADC_ZPX, ADCZeroPageX) 	// Zero Page X
		SWITCH_INS(INS_ADC_ABS, ADCAbsolute)  	// Absolute
		SWITCH_INS(INS_ADC_ABSX, ADCAbsoluteX)	// Absolute X
		SWITCH_INS(INS_ADC_ABSY, ADCAbsoluteY)	// Absolute Y
		SWITCH_INS(INS_ADC_INDX, ADCIndirectX)	// Indirect X
		SWITCH_INS(INS_ADC_INDY, ADCIndirectY)	// Indirect Y

		SWITCH_INS(INS_SBC_IM, SBCImmediate)  	// Immediate
		SWITCH_INS(INS_SBC_ZP, SBCZeroPage)		// Zero Page
		SWITCH_INS(INS_SBC_ZPX, SBCZeroPageX) 	// Zero Page X
		SWITCH_INS(INS_SBC_ABS, SBCAbsolute)  	// Absolute
		SWITCH_INS(INS_SBC_ABSX, SBCAbsoluteX)	// Absolute X
		SWITCH_INS(INS_SBC_ABSY, SBCAbsoluteY)	// Absolute Y
		SWITCH_INS(INS_SBC_INDX, SBCIndirectX)	// Indirect X
		SWITCH_INS(INS_SBC_INDY, SBCIndirectY)	// Indirect Y
		SWITCH_INS(INS_AND_IM, ANDImmediate)  	// Immediate
		SWITCH_INS(INS_AND_ZP, ANDZeroPage)		// Zero Page
		SWITCH_INS(INS_AND_ZPX, ANDZeroPageX)	// Zero Page X
		SWITCH_INS(INS_AND_ABS, ANDAbsolute)	// Absolute
		SWITCH_INS(INS_AND_ABSX, ANDAbsoluteX)	// Absolute X
		SWITCH_INS(INS_AND_ABSY, ANDAbsoluteY)	// Absolute Y
		SWITCH_INS(INS_AND_INDX, ANDIndirectX)	// Indirect X
		SWITCH_INS(INS_AND_INDY, ANDIndirectY)	// Indirect Y
		SWITCH_INS(INS_ASL_ACC, ASLAccumulator)	// Accumulator
		SWITCH_INS(INS_ASL_ZP, ASLZeroPage)		// Zero Page
		SWITCH_INS(INS_ASL_ZPX, ASLZeroPageX) 	// Zero Page X
		SWITCH_INS(INS_ASL_ABS, ASLAbsolute)  	// Absolute
		SWITCH_INS(INS_ASL_ABSX, ASLAbsoluteX)	// Absolute X
		SWITCH_INS(INS_EOR_IM, EORImmediate)  	// Immediate
		SWITCH_INS(INS_EOR_ZP, EORZeroPage)		// Zero Page
		SWITCH_INS(INS_EOR_ZPX, EORZeroPageX)	// Zero Page X
		SWITCH_INS(INS_EOR_ABS, EORAbsolute)	// Absolute
		SWITCH_INS(INS_EOR_ABSX, EORAbsoluteX)	// Absolute X
		SWITCH_INS(INS_EOR_ABSY, EORAbsoluteY)	// Absolute Y
		SWITCH_INS(INS_EOR_INDX, EORIndirectX)	// Indirect X
		SWITCH_INS(INS_EOR_INDY, EORIndirectY)	// Indirect Y
		SWITCH_INS(INS_LSR_ACC, LSRAccumulator)	// Accumulator
		SWITCH_INS(INS_LSR_ZP, LSRZeroPage)		// Zero Page
		SWITCH_INS(INS_LSR_ZPX, LSRZeroPageX)	// Zero Page X
		SWITCH_INS(INS_LSR_ABS, LSRAbsolute)	// Absolute
		SWITCH_INS(INS_LSR_ABSX, LSRAbsoluteX)	// Absolute X
		SWITCH_INS(INS_ORA_IM, ORAImmediate)	// Immediate
		SWITCH_INS(INS_ORA_ZP, ORAZeroPage)		// Zero Page
		SWITCH_INS(INS_ORA_ZPX, ORAZeroPageX)  	// Zero Page X
		SWITCH_INS(INS_ORA_ABS, ORAAbsolute)   	// Absolute
		SWITCH_INS(INS_ORA_ABSX, ORAAbsoluteX) 	// Absolute X
		SWITCH_INS(INS_ORA_ABSY, ORAAbsoluteY) 	// Absolute Y
		SWITCH_INS(INS_ORA_INDX, ORAIndirectX) 	// Indirect X
		SWITCH_INS(INS_ORA_INDY, ORAIndirectY) 	// Indirect Y
		SWITCH_INS(INS_ROL_ACC, ROLAccumulator)	// Accumulator
		SWITCH_INS(INS_ROL_ZP, ROLZeroPage)		// Zero Page
		SWITCH_INS(INS_ROL_ZPX, ROLZeroPageX)  	// Zero Page X
		SWITCH_INS(INS_ROL_ABS, ROLAbsolute)   	// Absolute
		SWITCH_INS(INS_ROL_ABSX, ROLAbsoluteX) 	// Absolute X
		SWITCH_INS(INS_ROR_ACC, RORAccumulator)	// Accumulator
		SWITCH_INS(INS_ROR_ZP, RORZeroPage)		// Zero Page
		SWITCH_INS(INS_ROR_ZPX, RORZeroPageX) 	// Zero Page X
		SWITCH_INS(INS_ROR_ABS, RORAbsolute)  	// Absolute
		SWITCH_INS(INS_ROR_ABSX, RORAbsoluteX)	// Absolute X

		SWITCH_INS(INS_CLC_IMP, CLCImplied)		// Implied
		SWITCH_INS(INS_CLD_IMP, CLDImplied)		// Implied
		SWITCH_INS(INS_CLI_IMP, CLIImplied)		// Implied
		SWITCH_INS(INS_CLV_IMP, CLVImplied)		// Implied
		SWITCH_INS(INS_SEC_IMP, SECImplied)		// Implied
		SWITCH_INS(INS_SED_IMP, SEDImplied)		// Implied
		SWITCH_INS(INS_SEI_IMP, SEIImplied)		// Implied

		SWITCH_INS(INS_BCC_REL, BCCImplied)		// Relative
		SWITCH_INS(INS_BCS_REL, BCSImplied)		// Relative
		SWITCH_INS(INS_BEQ_REL, BEQImplied)		// Relative
		SWITCH_INS(INS_BMI_REL, BMIImplied)		// Relative
		SWITCH_INS(INS_BNE_REL, BNEImplied)		// Relative
		SWITCH_INS(INS_BPL_REL, BPLImplied)		// Relative
		SWITCH_INS(INS_BVC_REL, BVCImplied)		// Relative
		SWITCH_INS(INS_BVS_REL, BVSImplied)		// Relative
		SWITCH_INS(INS_CMP_IM, CMPImmediate)	// Immediate
		SWITCH_INS(INS_CMP_ZP, CMPZeroPage)		// Zero Page
		SWITCH_INS(INS_CMP_ZPX, CMPZeroPageX) 	// Zero Page X
		SWITCH_INS(INS_CMP_ABS, CMPAbsolute)  	// Absolute
		SWITCH_INS(INS_CMP_ABSX, CMPAbsoluteX)	// Absolute X
		SWITCH_INS(INS_CMP_ABSY, CMPAbsoluteY)	// Absolute Y
		SWITCH_INS(INS_CMP_INDX, CMPIndirectX)	// Indirect X
		SWITCH_INS(INS_CMP_INDY, CMPIndirectY)	// Indirect Y
		SWITCH_INS(INS_CPX_IM, CPXImmediate)  	// Immediate
		SWITCH_INS(INS_CPX_ZP, CPXZeroPage)		// Zero Page
		SWITCH_INS(INS_CPX_ABS, CPXAbsolute)	// Absolute
		SWITCH_INS(INS_CPY_IM, CPYImmediate)	// Immediate
		SWITCH_INS(INS_CPY_ZP, CPYZeroPage)		// Zero Page
		SWITCH_INS(INS_CPY_ABS, CPYAbsolute)	// Absolute
		SWITCH_INS(INS_BRK_IMP, BRKImplied)		// Implied
		SWITCH_INS(INS_JMP_ABS, JMPAbsolute)	// Absolute
		SWITCH_INS(INS_JMP_IND, JMPIndirect)	// Indirect
		SWITCH_INS(INS_JSR_ABS, JSRAbsolute)	// 6
		SWITCH_INS(INS_RTI_IMP, RTIImplied)		// Implied
		SWITCH_INS(INS_RTS_IMP, RTSImplied)		// Implied
		SWITCH_INS(INS_NOP_IMP, NOPImplied)		// Implied
		SWITCH_INS(INS_BIT_ZP, BITZeroPage)		// Zero Page
		SWITCH_INS(INS_BIT_ABS, BITAbsolute)	// Absolute

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
		});
}

void CPU::PushZeroPageX()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			m_Calculated = (BYTE)(DataBus + X);
		});
}

void CPU::PushZeroPageY()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			m_Calculated = (BYTE)(DataBus + Y);
		});
}

void CPU::PushAbsolute()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_PCL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
		});
}

void CPU::PushIndirectX()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
		});
	m_InstructionQueue.push([&]()
		{
			m_Calculated = (BYTE)(DataBus + X);
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_Calculated;
			SetDataBusFromMemory();
			m_PCL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_Calculated + 1;
			SetDataBusFromMemory();
		});
}

void CPU::PushIndirectY()
{
	m_InstructionQueue.push([&]()
		{
			AddressBus = PC++;
			SetDataBusFromMemory();
			m_Calculated = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_Calculated;
			SetDataBusFromMemory();
			m_PCL = DataBus;
		});
	m_InstructionQueue.push([&]()
		{
			AddressBus = m_Calculated + 1;
			SetDataBusFromMemory();
		});
}

void CPU::SetA()
{
	SetDataBusFromMemory();
	A = DataBus;
	PS.Z = A == 0;
	PS.N = (A & BIT(7)) > 0;
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
	PS.Z = X == 0;
	PS.N = (X & BIT(7)) > 0;
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
	PS.Z = Y == 0;
	PS.N = (Y & BIT(7)) > 0;
}

void CPU::StoreY()
{
	DataBus = Y;
	WriteMemoryFromDataBus();
}

// -----------------------------------------------------------------------------
// ----------------------------- INSTRUCTIONS ----------------------------------
// -----------------------------------------------------------------------------

// ADC
// CLC
// CLD



void CPU::TAXImplied()
{
}

void CPU::TAYImplied()
{
}

void CPU::TSXImplied()
{
}

void CPU::TXAImplied()
{
}

void CPU::TXSImplied()
{
}

void CPU::TYAImplied()
{
}

void CPU::PHAImplied()
{
}

void CPU::PHPImplied()
{
}

void CPU::PLAImplied()
{
}

void CPU::PLPImplied()
{
}

void CPU::DECZeroPage()
{
}

void CPU::DECZeroPageX()
{
}

void CPU::DECAbsolute()
{
}

void CPU::DECAbsoluteX()
{
}

void CPU::INCZeroPage()
{
}

void CPU::INCZeroPageX()
{
}

void CPU::INCAbsolute()
{
}

void CPU::INCAbsoluteX()
{
}

void CPU::DEXImplied()
{
}

void CPU::DEYImplied()
{
}

void CPU::INXImplied()
{
}

void CPU::INYImplied()
{
}

void CPU::ADCImmediate()
{
}

void CPU::ADCZeroPage()
{
}

void CPU::ADCZeroPageX()
{
}

void CPU::ADCAbsolute()
{
}

void CPU::ADCAbsoluteX()
{
}

void CPU::ADCAbsoluteY()
{
}

void CPU::ADCIndirectX()
{
}

void CPU::ADCIndirectY()
{
}

void CPU::SBCImmediate()
{
}

void CPU::SBCZeroPage()
{
}

void CPU::SBCZeroPageX()
{
}

void CPU::SBCAbsolute()
{
}

void CPU::SBCAbsoluteX()
{
}

void CPU::SBCAbsoluteY()
{
}

void CPU::SBCIndirectX()
{
}

void CPU::SBCIndirectY()
{
}

void CPU::ANDImmediate()
{
}

void CPU::ANDZeroPage()
{
}

void CPU::ANDZeroPageX()
{
}

void CPU::ANDAbsolute()
{
}

void CPU::ANDAbsoluteX()
{
}

void CPU::ANDAbsoluteY()
{
}

void CPU::ANDIndirectX()
{
}

void CPU::ANDIndirectY()
{
}

void CPU::EORImmediate()
{
}

void CPU::EORZeroPage()
{
}

void CPU::EORZeroPageX()
{
}

void CPU::EORAbsolute()
{
}

void CPU::EORAbsoluteX()
{
}

void CPU::EORAbsoluteY()
{
}

void CPU::EORIndirectX()
{
}

void CPU::EORIndirectY()
{
}

void CPU::ORAImmediate()
{
}

void CPU::ORAZeroPage()
{
}

void CPU::ORAZeroPageX()
{
}

void CPU::ORAAbsolute()
{
}

void CPU::ORAAbsoluteX()
{
}

void CPU::ORAAbsoluteY()
{
}

void CPU::ORAIndirectX()
{
}

void CPU::ORAIndirectY()
{
}

void CPU::ASLAccumulator()
{
}

void CPU::ASLZeroPage()
{
}

void CPU::ASLZeroPageX()
{
}

void CPU::ASLAbsolute()
{
}

void CPU::ASLAbsoluteX()
{
}

void CPU::LSRAccumulator()
{
}

void CPU::LSRZeroPage()
{
}

void CPU::LSRZeroPageX()
{
}

void CPU::LSRAbsolute()
{
}

void CPU::LSRAbsoluteX()
{
}

void CPU::ROLAccumulator()
{
}

void CPU::ROLZeroPage()
{
}

void CPU::ROLZeroPageX()
{
}

void CPU::ROLAbsolute()
{
}

void CPU::ROLAbsoluteX()
{
}

void CPU::RORAccumulator()
{
}

void CPU::RORZeroPage()
{
}

void CPU::RORZeroPageX()
{
}

void CPU::RORAbsolute()
{
}

void CPU::RORAbsoluteX()
{
}

void CPU::CLCImplied()
{
}

void CPU::CLDImplied()
{
}

void CPU::CLIImplied()
{
}

void CPU::CLVImplied()
{
}

void CPU::SECImplied()
{
}

void CPU::SEDImplied()
{
}

void CPU::SEIImplied()
{
}

void CPU::BCCImplied()
{
}

void CPU::BCSImplied()
{
}

void CPU::BEQImplied()
{
}

void CPU::BMIImplied()
{
}

void CPU::BNEImplied()
{
}

void CPU::BPLImplied()
{
}

void CPU::BVCImplied()
{
}

void CPU::BVSImplied()
{
}

void CPU::CMPImmediate()
{
}

void CPU::CMPZeroPage()
{
}

void CPU::CMPZeroPageX()
{
}

void CPU::CMPAbsolute()
{
}

void CPU::CMPAbsoluteX()
{
}

void CPU::CMPAbsoluteY()
{
}

void CPU::CMPIndirectX()
{
}

void CPU::CMPIndirectY()
{
}

void CPU::CPXImmediate()
{
}

void CPU::CPXZeroPage()
{
}

void CPU::CPXAbsolute()
{
}

void CPU::CPYImmediate()
{
}

void CPU::CPYZeroPage()
{
}

void CPU::CPYAbsolute()
{
}

void CPU::BRKImplied()
{
}

void CPU::JMPAbsolute()
{
}

void CPU::JMPIndirect()
{
}

void CPU::JSRAbsolute()
{
	//WORD addressSR = FetchPCWord();
	//WriteWord(0x0100 | SP, PC - 1);
	//SP += 2;
	//PC = addressSR;
}

void CPU::RTIImplied()
{
}

void CPU::RTSImplied()
{
}

void CPU::NOPImplied()
{
}

void CPU::BITZeroPage()
{
}

void CPU::BITAbsolute()
{
}


