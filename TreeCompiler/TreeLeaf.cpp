#include "Tree.h"

namespace TreeCompiler
{
	namespace Tree
	{
		TreeLeaf::TreeLeaf(vector<Triangle*> triangles)
		{
			this->members = triangles;
			if (members.size() == 0)
			{
				int bp = 1;
			}
		}

		void TreeLeaf::Serialise(std::fstream& stream)
		{
			WriteLine(stream, "LEAF");
			WriteLine(stream, this->members.size());
			for (unsigned int t = 0; t < this->members.size(); t++)
			{
				WriteLine(stream, this->members[t]->index);
			}
		}
	}
}