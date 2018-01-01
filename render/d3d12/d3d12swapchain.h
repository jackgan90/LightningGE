#pragma once
#include <d3dx12.h>
#include <DXGI.h>
#include <dxgi1_4.h>
#include <wrl\client.h>
#include <unordered_map>
#include "iswapchain.h"
#include "d3d12rendertargetmanager.h"

namespace LightningGE
{
	namespace Render
	{
		using Microsoft::WRL::ComPtr;
		class LIGHTNINGGE_RENDER_API D3D12SwapChain : public ISwapChain
		{
		public:
			friend class D3D12Renderer;
			//we have to use raw pointer,because at the time the swap chain is created, D3D12Renderer is not constructed successfully yet
			//so there's actually no shared pointer pointed to it.Passing a smart pointer here will cause error
			D3D12SwapChain(const ComPtr<IDXGISwapChain3>& pSwapChain, D3D12Renderer* pRenderer);
			~D3D12SwapChain()override;
			bool Present()override;
			std::size_t GetBufferCount() const override{ return m_bufferCount; }
			SharedRenderTargetPtr GetBufferRenderTarget(unsigned int bufferIndex)override;
		private:
			void BindRenderTargets();
			unsigned int m_bufferCount;
			D3D12Renderer* m_renderer;
			ComPtr<IDXGISwapChain3> m_swapChain;
			std::unordered_map<UINT, RenderTargetID> m_renderTargets;
		};
	}
}