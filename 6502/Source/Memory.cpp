#include "Memory.h"

#include "Log.h"

#include <fstream>

Memory::Memory(uint32_t size, WORD zeroAddress, bool isROM)
	: m_Size(size), m_ZeroAddress(zeroAddress), m_IsROM(isROM)
{
	Reset();
}

Memory::~Memory()
{ 
	delete m_Data;
}

void Memory::Reset()
{
	m_Data = new std::vector<BYTE>(m_Size);
}

void Memory::LoadProgram(BYTE program[])
{
	for (int i = 0; i < m_Size; i++)
		WriteByte(m_ZeroAddress + i, program[i]);
			
	LOG_INFO("Program loaded.");
}

void Memory::LoadProgram(const char* filepath)
{
	std::ifstream file(filepath, std::ios::binary);

	file.unsetf(std::ios::skipws);
	std::streampos fileSize;

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<BYTE> program;
	program.reserve(fileSize);
	program.insert(program.begin(), std::istream_iterator<BYTE>(file), std::istream_iterator<BYTE>());

	if (program.size() != m_Size)
	{
		LOG_ERROR("Loaded program must be of same size as EEPROM ({0} KB)", m_Size / 1024);
		return;
	}

	std::swap(*m_Data, program);
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
