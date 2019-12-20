#include "engine.hpp"

int main(int argc, char **argv)
{
	/* game logic object */
	EGame control;	
	if (!control.status())
		return -1;
		
	GPlayer player(control);
	
	/* main game loop */
	while(control.window.isOpen())
	{
		while(control.window.waitEvent(control.eventHandler))
		{
			switch (control.eventHandler.type)
			{
				case sf::Event::Resized:
				case sf::Event::GainedFocus:
					control.render();
					break;
				
				case sf::Event::KeyPressed:
					player.action();
					control.map_physics();
					
					control.render();
					break;
					
				case sf::Event::Closed:
					control.window.close();
					return 0;
					break;
					
				default:
					break;
			};
		}
	}
	return 0;
}
