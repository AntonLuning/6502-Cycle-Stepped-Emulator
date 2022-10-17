#include "Memory.h"

#include "Log.h"

void Memory::Reset()
{
	m_Data = new std::vector<BYTE>(m_Size, 0);
}

void Memory::LoadProgram(BYTE program[])
{
	for (int i = 0; i < m_Size; i++)
		WriteByte(m_ZeroAddress + i, program[i]);
			
	LOG_INFO("Program loaded.");
}

bool Memory::IsAddressOk(const uint32_t& address)
{
	int32_t localAddress = address - m_ZeroAddress;

	return !(localAddress < 0 || localAddress >= m_Size);
}

BYTE Memory::ReadByte(const WORD& address)
{
	if (!IsAddressOk(address))
		return 0;

	return m_Data->at(address - m_ZeroAddress);
}

void Memory::WriteByte(const WORD& address, const BYTE& value)
{
	if (!IsAddressOk(address))
		return;

	m_Data->at(address - m_ZeroAddress) = value;
}

void Memory::ChangeMemory(uint32_t size, WORD zeroAddress)
{
	m_Size = size;
	m_ZeroAddress = zeroAddress;

	Reset();
}
