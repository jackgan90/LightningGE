#pragma once
#include "RenderPass.h"
#include "ThreadLocalObject.h"

namespace Lightning
{
	namespace Render
	{
		class ForwardRenderPass : public RenderPass
		{
		public:	
			ForwardRenderPass(IRenderer& renderer);
			std::size_t GetRenderTargetCount()const override;
			std::shared_ptr<IRenderTarget> GetRenderTarget(std::size_t index)const override;
			std::shared_ptr<IDepthStencilBuffer> GetDepthStencilBuffer()const override;
		protected:
			void DoRender()override;
			bool AcceptDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)override;
			Foundation::ThreadLocalObject<std::uint64_t> mLastRenderFrame;
			ColorF mClearColor;
		};
	}
}
