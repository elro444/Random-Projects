#pragma once
#include <cmath>

#include "TrackSamples.h"
#include "ChannelSamples.h"

class Filterer
{
public:
	Filterer(float threshold, size_t kernelSize);

	template<typename T>
	ChannelSamples<T> filter(const ChannelSamples<T>& channel, bool optimize = false)
	{
		ChannelSamples<T> filtered;
		std::vector<bool> shouldIncludeMap(channel.size(), false);
		T maximum = channel.getMax();
		T threshold = maximum * m_threshold;

		int lastNoisySample = -1;
		for (size_t i = 0; i < channel.size(); i++)
		{
			/*
			SIZE = 4 (including the current pos)
			0 1 2 3 4 5 6 7 8 (index)
			1 0 0 0 0 0 0 0 1 (original)
			1 1 1 1 0 1 1 1 1 (with margins)

			1 0 0 0 0 1 (for optimizations)
			1 1 1 1
			        1 1

			*/

			if (i - lastNoisySample < m_kernelSize)
			{
				if (optimize)
					filtered.push_back(channel[i]);
				shouldIncludeMap[i] = true;
			}

			if (abs(channel[i]) >= threshold)
			{
				shouldIncludeMap[i] = true;
				//filtered.push_back(channel[i]);
				int distanceToLastNoisy = i - lastNoisySample;
				if (distanceToLastNoisy > m_kernelSize)
				{
					int toChangeCount = m_kernelSize;
					for (size_t j = i + 1 - toChangeCount; j < i; j++)
					{
						if (optimize)
							filtered.push_back(channel[j]);
						shouldIncludeMap[j] = true;
					}
				}
				lastNoisySample = i;
			}
		}
		if (!optimize)
			for (size_t i = 0; i < shouldIncludeMap.size(); i++)
				if (shouldIncludeMap[i])
					filtered.push_back(channel[i]);
		return filtered;
	}

private:
	float m_threshold;
	size_t m_kernelSize;

};

