#pragma once
#include "iswapchain.h"
#include <DXGI.h>
#include <dxgi1_4.h>
#include <wrl\client.h>

namespace LightningGE
{
	namespace Renderer
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDERER_API D3D12SwapChain : public ISwapChain
		{
		public:
			friend class D3D12RenderContext;
			D3D12SwapChain(ComPtr<IDXGISwapChain3> pSwapChain);
			~D3D12SwapChain()override;
			bool Present()override;
			void ReleaseRenderResources()override;
			unsigned int GetBufferCount() const override{ return m_bufferCount; }
		private:
			unsigned int m_bufferCount;
			ComPtr<IDXGISwapChain3> m_swapChain;
		};
	}
}