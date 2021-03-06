#include "ForwardRenderPass.h"
#include "Renderer.h"
#include "FrameMemoryAllocator.h"
#include "tbb/flow_graph.h"
#include "tbb/parallel_for.h"


namespace Lightning
{
	namespace Render
	{
		extern FrameMemoryAllocator g_RenderAllocator;
		ForwardRenderPass::ForwardRenderPass(IRenderer& renderer) 
			:RenderPass(renderer), mClearColor{0.5f, 0.5f, 0.5f, 1.0f}
		{

		}

		void ForwardRenderPass::DoRender()
		{
			auto backBuffer = mRenderer.GetDefaultRenderTarget();
			mRenderer.ClearRenderTarget(backBuffer.get(), mClearColor);
			auto depthStencilBuffer = mRenderer.GetDefaultDepthStencilBuffer();
			mRenderer.ClearDepthStencilBuffer(depthStencilBuffer.get(), DepthStencilClearFlags::CLEAR_DEPTH | DepthStencilClearFlags::CLEAR_STENCIL,
				depthStencilBuffer->GetDepthClearValue(), depthStencilBuffer->GetStencilClearValue(), nullptr);
			
			tbb::parallel_for(tbb::blocked_range<std::size_t>(0, mCurrentDrawList->size()), 
				[this](const tbb::blocked_range<std::size_t>& range) {
				auto currentFrame = mRenderer.GetCurrentFrameCount();
				auto& lastRenderFrame = mLastRenderFrame.Local();
				if (currentFrame != lastRenderFrame)
				{
					lastRenderFrame = currentFrame;
					auto backBuffer = mRenderer.GetDefaultRenderTarget();
					auto depthStencilBuffer = mRenderer.GetDefaultDepthStencilBuffer();
					auto renderTargets = g_RenderAllocator.Allocate<IRenderTarget*>(1);
					renderTargets[0] = backBuffer.get();
					mRenderer.ApplyRenderTargets(renderTargets, 1, depthStencilBuffer.get());

					auto scissorRects = g_RenderAllocator.Allocate<ScissorRect>(1);
					auto viewports = g_RenderAllocator.Allocate<Viewport>(1);
					auto window = mRenderer.GetOutputWindow();

					viewports[0].left = viewports[0].top = .0f;
					viewports[0].width = static_cast<float>(window->GetWidth());
					viewports[0].height = static_cast<float>(window->GetHeight());

					scissorRects[0].left = static_cast<long>(viewports[0].left);
					scissorRects[0].top = static_cast<long>(viewports[0].top);
					scissorRects[0].width = static_cast<long>(viewports[0].width);
					scissorRects[0].height = static_cast<long>(viewports[0].height);
					mRenderer.ApplyViewports(viewports, 1);
					mRenderer.ApplyScissorRects(scissorRects, 1);
				}
				for (std::size_t i = range.begin(); i != range.end();++i)
				{
					DrawableElement element = mCurrentDrawList->at(i);
					auto drawCommand = NewDrawCommand();
					drawCommand->SetPrimitiveType(element.drawable->GetPrimitiveType());
					drawCommand->SetIndexBuffer(element.drawable->GetIndexBuffer());
					drawCommand->SetVertexBuffers(element.drawable->GetVertexBuffers());
					drawCommand->SetMaterial(element.drawable->GetMaterial());
					drawCommand->SetTransform(element.drawable->GetDrawTransform());
					drawCommand->SetViewMatrix(element.camera->GetViewMatrix());
					drawCommand->SetProjectionMatrix(element.camera->GetProjectionMatrix());
					drawCommand->Commit();
				}
			});
		}

		bool ForwardRenderPass::AcceptDrawable(const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<ICamera>& camera)
		{
			return true;
		}

		std::size_t ForwardRenderPass::GetRenderTargetCount()const
		{
			return 1;
		}

		std::shared_ptr<IRenderTarget> ForwardRenderPass::GetRenderTarget(std::size_t index)const
		{
			return mRenderer.GetDefaultRenderTarget();
		}

		std::shared_ptr<IDepthStencilBuffer> ForwardRenderPass::GetDepthStencilBuffer()const
		{
			return mRenderer.GetDefaultDepthStencilBuffer();
		}
	}
}