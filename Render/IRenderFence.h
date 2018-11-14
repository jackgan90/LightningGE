#pragma once
#include <cstdint>

namespace Lightning
{
	namespace Render
	{
		class IRenderFence
		{
		public:
			virtual ~IRenderFence(){}
			virtual void SetTargetValue(std::uint64_t value) = 0;
			virtual std::uint64_t GetTargetValue() = 0;
			virtual std::uint64_t GetCompletedValue() = 0;
			virtual void WaitForTarget() = 0;
		};
	}
}