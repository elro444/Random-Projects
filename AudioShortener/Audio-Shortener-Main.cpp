#include <sstream>
#include <iomanip>
#include <string>
#include <assert.h>
#include <SFML/Graphics.hpp>
#include "AudioPlayer.h"
#include "WAVParser.h"
#include "arialFontHeader.h"
#include "ChannelSamples.h"
#include "Filterer.h"

// For a program with no console, swap 'int main()' with this line: [requires Windows.h]
//int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPTSTR, _In_ INT)

namespace std
{
	string to_string(sf::Vector2f v)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << "(" << v.x << ", " << v.y << ")";
		return stream.str();
	}
}

template<typename T>
std::vector<sf::Vertex> plotGraph(const ChannelSamples<T>& channel, float y);

template<typename T>
ChannelSamples<T> manualFilter(const ChannelSamples<T>& samples, float threshold);

int main()
{
	WAVParser parser("D:\\Desktop\\Alarm07.wav");

	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Audio-Shortener", sf::Style::Default, sf::ContextSettings(0, 0, 8));

	sf::Font arialFont;
	arialFont.loadFromMemory(arialFontArray, arialFontArraySize);

	sf::View view(window.getView());

	std::vector<sf::Color> colors{
		sf::Color::White,
		sf::Color(196, 196, 196)
	};

	std::vector<std::vector<sf::Vertex>> graphs;
	size_t samplesCount = parser.dataChunk.chunkSize / (parser.fmtChunk.bitsPerSample / 8 * parser.fmtChunk.channels);
	ChannelSamples<short> samples(parser.dataChunk.data.data(), 0, 2, samplesCount);
	short maxSample = samples.getMax();
	float yDiff = maxSample * 2.f + 500;
	float threshold = 0.1f;

	//assert(manualFilter(samples, threshold) == Filterer(threshold, 0).filter(samples));

	graphs.push_back(plotGraph(samples, yDiff * graphs.size()));
	graphs.push_back(plotGraph(manualFilter(samples, threshold), yDiff * graphs.size()));
	for (int i : {1, 3, 5, 10, 50, 100, 1000})
	{
		Filterer f(threshold, i);
		graphs.push_back(plotGraph(f.filter(samples), yDiff * graphs.size()));
		assert(f.filter(samples) == f.filter(samples, true));
	}



	sf::Clock clock;
	while (window.isOpen())
	{
		if (view.getSize() == sf::Vector2f())
			view = window.getView();
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
		sf::Time dt = clock.restart();

		sf::Vector2f offset(
			sf::Keyboard::isKeyPressed(sf::Keyboard::Right) - sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
			sf::Keyboard::isKeyPressed(sf::Keyboard::Down) - sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
		);
		float zoom = sf::Keyboard::isKeyPressed(sf::Keyboard::Z) - sf::Keyboard::isKeyPressed(sf::Keyboard::X);
		if (offset != sf::Vector2f() || zoom)
		{
			view.move(view.getSize().y * offset * dt.asSeconds());
			view.zoom(1 + zoom * dt.asSeconds());
			window.setView(view);
		}

		window.clear();
		/*sf::CircleShape c(100, 60);
		c.setPosition(0, 0);
		c.setFillColor(sf::Color::Cyan);
		window.draw(c);*/
		for (const std::vector<sf::Vertex>& graph : graphs)
			window.draw(graph.data(), graph.size(), sf::LineStrip);
		window.display();
	}

	//auto& fmt = parser.fmtChunk;
	//AudioPlayer ap(fmt.samplesPerSec, fmt.bitsPerSample, fmt.channels, fmt.formatTag, fmt.blockAlign, fmt.avgBytesPerSec);
	//getchar();
	return 0;
}

template<typename T>
std::vector<sf::Vertex> plotGraph(const ChannelSamples<T>& samples, float y)
{
	std::vector<sf::Vertex> graph;
	sf::Vector2f scale(1, 1);
	sf::Color color = sf::Color::White;

	graph.reserve(samples.size());
	for (size_t i = 0; i < samples.size(); i++)
	{
		graph.push_back(sf::Vertex(sf::Vector2f(i * scale.x, y + samples[i] * scale.y), color));
	}
	return graph;
}

template<typename T>
ChannelSamples<T> manualFilter(const ChannelSamples<T>& samples, float threshold)
{
	std::vector<T> newSamples;
	T maxVal = samples.getMax();
	for (const T& elem : samples)
		if (std::abs(elem) > maxVal * threshold)
			newSamples.push_back(elem);
	return ChannelSamples<T>(newSamples);
}
