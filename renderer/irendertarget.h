#pragma once
#include <memory>
#include "rendererexportdef.h"

namespace LightningGE
{
	namespace Renderer
	{
		typedef int RenderTargetID;
		class LIGHTNINGGE_RENDERER_API IRenderTarget
		{
		public:
			//return if the render target representing a swap chain buffer(a back buffer)
			virtual bool IsSwapChainRenderTarget()const = 0;
			//return the attached RT ID
			virtual RenderTargetID GetID()const = 0;
			virtual ~IRenderTarget() = default;
		};
		typedef std::shared_ptr<IRenderTarget> RenderTargetPtr;
	}
}