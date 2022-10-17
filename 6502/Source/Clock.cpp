#include "Clock.h"

void Clock::UpdateClock()
{
	m_NextCycleTime = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(m_Speed);
}

void Clock::Start()
{
	m_Running = true;
	UpdateClock();
}

void Clock::Stop() 
{ 
	m_Running = false; 
}

void Clock::WaitForNextCycle()
{
	std::this_thread::sleep_until(m_NextCycleTime);
	m_NextCycleTime += std::chrono::milliseconds(m_Speed);
}

void Clock::SetSpeedHZ(uint32_t speed)
{
	SetSpeedMS(1000 / speed);
}

void Clock::SetSpeedMS(uint32_t speed)
{
	m_Speed = speed;
	UpdateClock();
	LOG_INFO("Clock speed set to {0} Hz ({1} ms per cycle)", 1000 / m_Speed, m_Speed);
}
