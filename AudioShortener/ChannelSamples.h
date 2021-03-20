#pragma once
#include <vector>

template<typename T>
class ChannelSamples : public std::vector<T>
{
public:
	ChannelSamples() : vector()
	{
	}

	ChannelSamples(std::vector<T> other) :
		vector(other)
	{
	}

	ChannelSamples(void* data, size_t channel, size_t channelsCount, size_t samplesCount)
	{
		T* T_data = static_cast<T*>(data);
		reserve(samplesCount);
		for (size_t i = 0; i < samplesCount; i++)
		{
			push_back(*(T_data + channel + channelsCount * i));
		}
	}

	T getMax() const
	{
		T maximum = *begin();
		for (const T& elem : *this)
			if (std::abs(elem) > maximum)
				maximum = std::abs(elem);
		return maximum;
	}
};
