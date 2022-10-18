#pragma once

#include "Base.h"

#include <vector>

class Memory
{
public:
	Memory(uint32_t size, WORD zeroAddress, bool isROM);
	~Memory();

	void Reset();

	void LoadProgram(BYTE program[]);	// TEMP
	void LoadProgram(const char* filepath);
	
	inline bool IsROM() { return m_IsROM; }
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

