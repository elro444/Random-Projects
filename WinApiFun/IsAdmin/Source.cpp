#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "GlobalUtil/WinAPIUtil.h"
#include "IsAdmin.h"
#include "NotAdmin.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	bool hasAdminRights = Util::isAdmin();
	const char* image;
	unsigned length;
	if (hasAdminRights)
	{
		image = IsAdmin_png;
		length = IsAdmin_pngSize;
	}
	else
	{
		image = NotAdmin_png;
		length = NotAdmin_pngSize;
	}
	sf::Texture texture;
	texture.loadFromMemory(image, length);
	sf::Sprite sprite(texture);

	sf::RenderWindow window(sf::VideoMode(texture.getSize().x, texture.getSize().y), "HasAdminRights");
	window.setFramerateLimit(30);
	window.clear();
	window.draw(sprite);
	window.display();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
					window.close();
					break;
				}
				break;
			}
		}
	}
	return 0;
}