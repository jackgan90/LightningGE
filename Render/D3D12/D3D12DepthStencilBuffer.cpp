#include <d3dx12.h>
#include "D3D12DepthStencilBuffer.h"
#include "D3D12DescriptorHeapManager.h"
#include "D3D12TypeMapper.h"
#include "Renderer.h"

namespace Lightning
{
	namespace Render
	{
		D3D12DepthStencilBuffer::D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height):
			mDepthClearValue(1.0f), mStencilClearValue(0), mFormat(RenderFormat::D24_S8), mWidth(width), mHeight(height)
		{
			CreateResource();
		}

		D3D12DepthStencilBuffer::D3D12DepthStencilBuffer(const std::uint32_t width, const std::uint32_t height,
			RenderFormat format, const float depthClearValue, const std::uint32_t stencilClearValue):
			mDepthClearValue(depthClearValue), mStencilClearValue(stencilClearValue), mFormat(format) 
			,mWidth(width), mHeight(height)
		{
			CreateResource();
		}

		void D3D12DepthStencilBuffer::CreateResource()
		{
			//create heap
			mHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, false, 1, false);

			auto nativeFormat = D3D12TypeMapper::MapRenderFormat(mFormat);
			//create resource
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = nativeFormat;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE dsClearValue{};
			dsClearValue.Format = nativeFormat;
			dsClearValue.DepthStencil.Depth = mDepthClearValue;
			dsClearValue.DepthStencil.Stencil = mStencilClearValue;

			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			mResource = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(nativeFormat, mWidth, mHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsClearValue);
			device->CreateDepthStencilView((*mResource), &dsvDesc, mHeap->cpuHandle);
		}

		D3D12DepthStencilBuffer::~D3D12DepthStencilBuffer()
		{
			D3D12DescriptorHeapManager::Instance()->Deallocate(mHeap);
			mHeap = nullptr;
		}

		void D3D12DepthStencilBuffer::SetClearValue(float depthValue, std::uint32_t stencilValue)
		{

		}

		float D3D12DepthStencilBuffer::GetDepthClearValue()const
		{
			return mDepthClearValue;
		}

		std::uint32_t D3D12DepthStencilBuffer::GetStencilClearValue()const
		{
			return mStencilClearValue;
		}

		RenderFormat D3D12DepthStencilBuffer::GetRenderFormat()const
		{
			return mFormat;
		}

		void D3D12DepthStencilBuffer::TransitToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
		{
			mResource->TransitTo(commandList, state);
		}
	}
}