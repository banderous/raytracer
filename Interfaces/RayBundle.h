#pragma once

#include "emmintrin.h"

// Represents a bundle of rays; four.
struct RayBundle
{
	// Direction signs. Same as standard ray class
	int directionSignFlags[3];

	// The origin and reciprocal directions of the four rays this bundle represents
	__m128 rayOrigins[3], rayDirections[3], rayReciprocalDirections[3];

	// A validity mask for all four rays
	__m128 validity;

	// Indexes of triangles that are hit
	__m128 struckIndicies;

	// Distances at which the triangles were struck
	__m128 struckDistances;

	// Indexes of the pixels making up the bundle
	__m128 pixelXIndicies, pixelYIndicies;
};