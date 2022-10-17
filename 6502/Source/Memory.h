#pragma once

#include "Base.h"

#include <vector>

class Memory
{
public:
	Memory() {}
	Memory(uint32_t size, WORD zeroAddress, bool isROM)
		: m_Size(size), m_ZeroAddress(zeroAddress), m_IsROM(isROM) {}
	~Memory() { delete m_Data; }

	void Reset();

	void LoadProgram(BYTE program[]);
	
	bool IsAddressOk(const uint32_t& address);

	BYTE ReadByte(const WORD& address);
	void WriteByte(const WORD& address, const BYTE& value);

	void ChangeMemory(uint32_t size, WORD zeroAddress);

private:
	bool m_IsROM = true;
	uint32_t m_Size = 0;
	WORD m_ZeroAddress = 0;
	std::vector<BYTE>* m_Data = nullptr;
};

