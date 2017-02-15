#include "MathLib.h"
#include "Math.h"

namespace MathLib
{
	Ray::Ray(MathLib::C3Vector origin, MathLib::C3Vector direction, float magnitude)
	{
		this->origin = origin;
		this->direction = direction;
		this->magnitude = magnitude;

		this->directionSignFlags[0] = (direction.values[0] < 0) ? 0 : 1;
		this->directionSignFlags[1] = (direction.values[1] < 0) ? 0 : 1;
		this->directionSignFlags[2] = (direction.values[2] < 0) ? 0 : 1;

		this->reciprocalDirection = direction;
		this->reciprocalDirection.values[0] = 1.0f / this->reciprocalDirection.values[0];
		this->reciprocalDirection.values[1] = 1.0f / this->reciprocalDirection.values[1];
		this->reciprocalDirection.values[2] = 1.0f / this->reciprocalDirection.values[2];

	}
}