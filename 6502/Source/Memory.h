#pragma once

#include "Base.h"

#include <vector>

class Memory
{
public:
	Memory() : m_Size(0), m_ZeroAddress(0) {}
	Memory(uint32_t size, WORD zeroAddress) : m_Size(size), m_ZeroAddress(zeroAddress) {}
	~Memory() { delete m_Data; }

	void Reset();

	void LoadProgram(BYTE program[]);
	
	bool IsAddressOk(const uint32_t& address);

	BYTE ReadByte(const WORD& address);
	void WriteByte(const WORD& address, const BYTE& value);

	void ChangeMemory(uint32_t size, WORD zeroAddress);

private:
	uint32_t m_Size = 0;
	WORD m_ZeroAddress = 0;
	std::vector<BYTE>* m_Data = nullptr;
};

