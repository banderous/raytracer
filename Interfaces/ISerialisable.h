#pragma once

#include <fstream>

namespace Serialisation
{
	using namespace std;

	class ISerialisable
	{
		public:
			virtual void Deserialise(fstream& stream) = 0;
	};
}