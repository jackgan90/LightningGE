#pragma once
#include <memory>
#include <exception>
#include "irendertarget.h"

namespace LightningGE
{
	namespace Render
	{
		class LIGHTNINGGE_RENDER_API ISwapChain
		{
		public:
			virtual ~ISwapChain() = default;
			//preset the current back buffer to display
			virtual bool Present() = 0;
			//get swapchain back buffer count
			virtual std::size_t GetBufferCount()const = 0;
			//get corresponding render target
			virtual SharedRenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex) = 0;
		};
		using SharedSwapChainPtr = std::shared_ptr<ISwapChain>;
	}
}