#pragma once
#include <cstdint>
#include <memory>
#include "RenderConstants.h"
#include "IRefObject.h"

namespace Lightning
{
	namespace Render
	{
		enum class GPUBufferType
		{
			VERTEX,
			INDEX,
		};
		

		class IGPUBuffer : public Plugins::IRefObject
		{
		public:
			//get internal data
			virtual std::uint8_t* Lock(std::size_t start, std::size_t size) = 0;
			//set internal buffer,no copy
			virtual void Unlock(std::size_t start, std::size_t size) = 0;
			//commit the buffer to GPU
			virtual void Commit() = 0;
			//get internal buffer size in bytes
			virtual std::uint32_t GetBufferSize()const = 0;
			//get the buffer type
			virtual GPUBufferType GetType()const = 0;
		};
	}
}
