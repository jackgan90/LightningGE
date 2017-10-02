#include "d3d12rendertargetmanager.h"

namespace LightningGE
{
	namespace Renderer
	{
		D3D12RenderTargetManager::D3D12RenderTargetManager(DevicePtr pDevice, SwapChainPtr pSwapChain) :m_pDevice(pDevice), m_pSwapChain(pSwapChain)
			,m_currentID(1)
		{

		}

		RenderTargetPtr D3D12RenderTargetManager::CreateRenderTarget()
		{
			return RenderTargetPtr();
		}

		void D3D12RenderTargetManager::ReleaseRenderResources()
		{
			for (auto it : m_renderTargets)
				it.second->ReleaseRenderResources();
			m_renderTargets.clear();
			m_pDevice.reset();
			m_pSwapChain.reset();
		}
	}
}