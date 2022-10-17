#pragma once

#include "Base.h"
#include "Log.h"

#include <chrono>
#include <thread>

class Clock
{
private:
	bool m_Running = false;
	uint32_t m_Speed = 1000;	// In ms
	std::chrono::steady_clock::time_point m_NextCycleTime;

public:
	Clock() { UpdateClock(); }
	~Clock() = default;

	void UpdateClock();

	void Start();
	void Stop();
	// TODO: Implement Step()

	inline bool IsRunning() { return m_Running; }

	void WaitForNextCycle();

	void SetSpeedHZ(uint32_t speed);
	void SetSpeedMS(uint32_t speed);
};