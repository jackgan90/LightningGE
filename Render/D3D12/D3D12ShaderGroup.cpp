#include "D3D12Renderer.h"
#include "D3D12ShaderGroup.h"
#include "D3D12DescriptorHeapManager.h"

namespace Lightning
{
	namespace Render
	{
		D3D12ShaderGroup::D3D12ShaderGroup()
			: mConstantBufferCount(0)
			, mTextureCount(0)
			, mSamplerStateCount(0)
		{

		}

		D3D12ShaderGroup::~D3D12ShaderGroup()
		{
			Destroy();
		}

		void D3D12ShaderGroup::AddShader(D3D12Shader* shader)
		{
			if (!shader)
				return;
			shader->AddRef();
			mConstantBufferCount += shader->GetConstantBufferCount();
			mTextureCount += shader->GetTextureCount();
			mSamplerStateCount += shader->GetSamplerStateCount();
			mShaders.push_back(shader);
		}

		void D3D12ShaderGroup::Destroy()
		{
			for (auto shader : mShaders)
			{
				shader->Release();
			}
			mConstantBufferCount = 0;
			mTextureCount = 0;
			mSamplerStateCount = 0;
			mShaders.clear();
		}

		void D3D12ShaderGroup::Commit()
		{
			auto renderer = static_cast<D3D12Renderer*>(Renderer::Instance());
			auto device = static_cast<D3D12Device*>(renderer->GetDevice());
			auto commandList = renderer->GetGraphicsCommandList();
			auto frameResourceIndex = renderer->GetFrameResourceIndex();
			using DescriptorHeapLists = Foundation::ThreadLocalObject<Container::Vector<ID3D12DescriptorHeap*>>;
			static DescriptorHeapLists descriptorHeapLists;
			auto& descriptorHeaps = *descriptorHeapLists;
			descriptorHeaps.clear();
			//the heap to store CBVs SRVs UAVs
			DescriptorHeap* constantHeap{ nullptr };
			if (mConstantBufferCount + mTextureCount > 0)
			{
				constantHeap = D3D12DescriptorHeapManager::Instance()->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
					true, UINT(mConstantBufferCount + mTextureCount), true);
				descriptorHeaps.push_back(D3D12DescriptorHeapManager::Instance()->GetHeap(constantHeap).Get());
			}
			if (!descriptorHeaps.empty())
			{
				commandList->SetDescriptorHeaps(UINT(descriptorHeaps.size()), &descriptorHeaps[0]);
			}
			UINT rootParameterIndex{ 0 };
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle;
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
			if (constantHeap)
			{
				cpuHandle = constantHeap->cpuHandle;
				gpuHandle = constantHeap->gpuHandle;
			}
			for (auto shader : mShaders)
			{
				auto boundResources = shader->GetRootBoundResources();
				for (auto i = 0;i < shader->GetRootParameterCount();++i)
				{
					const auto& resource = boundResources[i];
					if (boundResources[i].type == D3D12RootResourceType::ConstantBuffers)
					{
						commandList->SetGraphicsRootDescriptorTable(rootParameterIndex++, gpuHandle);
						for (auto i = 0;i < resource.count;++i)
						{
							const auto& cbuffer = resource.buffers[i];
							D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
							cbvDesc.BufferLocation = cbuffer.virtualAdress;
							cbvDesc.SizeInBytes = UINT(cbuffer.size);
							device->CreateConstantBufferView(&cbvDesc, cpuHandle);
							cpuHandle.Offset(constantHeap->incrementSize);
							gpuHandle.Offset(constantHeap->incrementSize);
						}
					}
				}
			}
		}

		std::size_t D3D12ShaderGroup::GetHash()const
		{
			size_t seed = 0;
			boost::hash_combine(seed, mShaders.size());
			for (const auto& s : mShaders)
			{
				boost::hash_combine(seed, s->GetHash());
			}
			return seed;
		}

		ComPtr<ID3D12RootSignature> D3D12ShaderGroup::CreateRootSignature()
		{
			auto device = static_cast<D3D12Device*>(Renderer::Instance()->GetDevice());
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			Container::Vector<D3D12_ROOT_PARAMETER> rootParameters;
			D3D12_ROOT_SIGNATURE_FLAGS flags = 
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | 
				D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS | 
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
			for (std::size_t i = 0;i < mShaders.size();++i)
			{
				switch (mShaders[i]->GetType())
				{
				case ShaderType::VERTEX:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::FRAGMENT:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::GEOMETRY:
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::HULL:		//hull
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
					break;
				case ShaderType::DOMAIN:	//domain
					flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
					break;
				}
				const auto& parameters = static_cast<D3D12Shader*>(mShaders[i])->GetRootParameters();
				rootParameters.insert(rootParameters.end(), parameters.begin(), parameters.end());
			}
			D3D12_ROOT_PARAMETER* pParameters = rootParameters.empty() ? nullptr : &rootParameters[0];
			rootSignatureDesc.Init(UINT(rootParameters.size()), pParameters, 0, nullptr, flags);

			ComPtr<ID3DBlob> signature;
			auto hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
			if (FAILED(hr))
			{
				return ComPtr<ID3D12RootSignature>();
			}

			mRootSignature = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize());
			return mRootSignature;
		}
	}
}
