#include "Filterer.h"
#include <cmath>

Filterer::Filterer(float threshold, size_t kernelSize) : 
	m_threshold(threshold),
	m_kernelSize(kernelSize)
{
}