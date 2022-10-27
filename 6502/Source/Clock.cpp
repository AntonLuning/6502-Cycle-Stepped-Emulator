#include "Clock.h"

void Clock::Start()
{
	m_Running = true;
	UpdateClock();
}

void Clock::Stop() 
{ 
	m_Running = false; 
}

void Clock::Step()
{
	if (m_Running)
		LOG_ERROR("Can't step the clock while its running.");

	m_Running = true;
	m_Step = true;
}

void Clock::WaitForNextCycle()
{
	if (m_Step)
	{
		m_Running = false;
		m_Step = false;
	}
	else if (m_Speed != 0)
	{
		std::this_thread::sleep_until(m_NextCycleTime);
		m_NextCycleTime += std::chrono::microseconds((uint32_t)(m_Speed * 1000));
	}
}

void Clock::SetSpeedHZ(uint32_t speed)
{
	if (speed == 0)
	{
		LOG_ERROR("Can't set clock speed to 0 Hz. Use the \"Stop\" function if you desire to stop the clock.");
		return;
	}
	SetSpeedMS((float)1000 / speed);
}

void Clock::SetSpeedMS(float speed)
{
	if (speed < 0)
	{
		LOG_ERROR("The clock speed in MS must be a positive number.");
		return;
	}

	m_Speed = speed;
	if (m_Speed != 0)
	{
		UpdateClock();
		LOG_INFO("Clock speed set to {0} Hz ({1} ms per cycle)", (uint32_t)(1000 / m_Speed), m_Speed);
	}
	else
		LOG_INFO("Clock speed set to max speed)");
}

void Clock::UpdateClock()
{
	m_NextCycleTime = std::chrono::high_resolution_clock::now() + std::chrono::microseconds((uint32_t)(m_Speed * 1000));
}