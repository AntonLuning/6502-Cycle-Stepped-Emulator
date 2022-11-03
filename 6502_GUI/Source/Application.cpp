#include <Computer.h>
#include <Log.h>

#include <SFML/Graphics.hpp>

#define PROGRAM_LENGTH(prg) sizeof(prg) / sizeof(BYTE)

constexpr uint32_t SRAM_MEMORY = 32 * 1024;
constexpr uint32_t EEPROM_MEMORY = 16 * 1024;

// $0000 -> $7FFF : SRAM
// $8000 -> $BFFF : free addresses (e.g. for I/O)
// $C000 -> $FFFF : EEPROM

int main()
{
	Log::Init();

	Computer* computer = new Computer(SRAM_MEMORY, EEPROM_MEMORY);
	computer->EEPROM->LoadProgram("C:\\Dev\\6502\\6502_Tests\\TestPrograms\\program7.out");
	computer->clock.SetSpeedMS(20);

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Hello World");

	sf::CircleShape shape(200);
	shape.setFillColor(sf::Color(100, 200, 50));
	
	computer->clock.Start();
	
	while (window.isOpen()) 
	{
		sf::Event event;
		
		window.clear();
		window.draw(shape);
		window.display();

		computer->RunCycle();

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	delete computer;

	Log::Shutdown();
}