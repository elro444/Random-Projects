#pragma once
#include <vector>
#include "ChannelSamples.h"

template<typename T>
class TrackSamples
{
public:
	TrackSamples(void* data, size_t channelsCount, size_t samplesCount) :
		m_channels()
	{
		for (size_t i = 0; i < channelsCount; i++)
		{
			m_channels.push_back(ChannelSamples<T>(data, i, channelsCount, samplesCount));
		}
	}

private:
	std::vector<ChannelSamples<T>> m_channels;
};

