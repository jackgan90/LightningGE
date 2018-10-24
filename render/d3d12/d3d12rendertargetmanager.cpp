#include "d3d12rendertargetmanager.h"
#include "d3d12rendertarget.h"
#include "d3d12swapchain.h"

namespace Lightning
{
	namespace Render
	{
		D3D12RenderTargetManager::D3D12RenderTargetManager() :mCurrentID(1)
		{

		}

		D3D12RenderTargetManager::~D3D12RenderTargetManager()
		{
		}


		SharedRenderTargetPtr D3D12RenderTargetManager::CreateRenderTarget()
		{
			return SharedRenderTargetPtr();
		}

		SharedRenderTargetPtr D3D12RenderTargetManager::CreateSwapChainRenderTarget(
			const ComPtr<ID3D12Resource>& resource, D3D12SwapChain* pSwapChain)
		{
			RenderTargetID rtID = mCurrentID.fetch_add(1, std::memory_order_relaxed);
			auto ptr = SharedRenderTargetPtr(new D3D12RenderTarget(rtID, resource, pSwapChain));
			StoreRenderTarget(rtID, ptr);
			return ptr;
		}
	}
}