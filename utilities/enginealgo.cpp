#include <boost/functional/hash.hpp>
#include <functional>
#include "EngineAlgo.h"

namespace Lightning
{
	namespace Utility
	{
		std::size_t CalculateHash(const std::string& str, const Container::UnorderedMap<std::string, std::string>& map)
		{
			//TODO : do test about hash collision
			std::size_t seed = 0;
			boost::hash_combine(seed, str);
			for (auto it = map.begin(); it != map.end(); ++it)
			{
				boost::hash_combine(seed, it->first);
				boost::hash_combine(seed, it->second);
			}
			return seed;
		}
	}
}