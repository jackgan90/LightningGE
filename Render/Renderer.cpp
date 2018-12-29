#include <cassert>
#include "Device.h"
#include "Renderer.h"
#include "ForwardRenderPass.h"
#include "DeferedRenderPass.h"
#include "FrameMemoryAllocator.h"
#include "Serializers/ShaderSerializer.h"
#include "Serializers/TextureSerializer.h"
#include "RenderUnit.h"
#include "Loader.h"

namespace Lightning
{
	namespace Render
	{
		IRenderer* Renderer::sInstance{ nullptr };
		FrameMemoryAllocator g_RenderAllocator;
		
		void FrameResource::ReleaseRenderQueue()
		{
			if (renderQueue)
			{
				for (auto unit : *renderQueue)
				{
					unit->Release();
				}
				renderQueue->clear();
			}
		}

		void FrameResource::OnFrameBegin()
		{
			ReleaseRenderQueue();
		}

		void FrameResource::Release()
		{
			if (fence)
			{
				delete fence;
				fence = nullptr;
			}
			if (defaultDepthStencilBuffer)
				defaultDepthStencilBuffer->Release();
			ReleaseRenderQueue();
		}

		Renderer::Renderer(Window::IWindow* window)
			: mOutputWindow(window)
			, mFrameCount(0)
			, mFrameResourceIndex(0)
			, mClearColor{0.5f, 0.5f, 0.5f, 1.0f}
			, mRenderQueueIndex(RENDER_FRAME_COUNT)
			, mCurrentFrameRenderQueue(&mRenderQueues[RENDER_FRAME_COUNT])
			, mStarted(false)
		{
			assert(!sInstance);
			mOutputWindow->AddRef();
			sInstance = this;
			for (const auto& semanticItem : PipelineInputSemantics)
			{
				mPipelineInputSemanticInfos[semanticItem.semantic] = ParsePipelineInputSemantics(semanticItem);
			}
			for (const auto& uniformSemantic : UniformSemantics)
			{
				mUniformToSemantics[uniformSemantic.name] = uniformSemantic.semantic;
				mSemanticsToUniform[uniformSemantic.semantic] = uniformSemantic.name;
			}
		}

		Renderer::~Renderer()
		{
			assert(sInstance == this);
			sInstance = nullptr;
		}

		Renderer::SemanticInfo Renderer::ParsePipelineInputSemantics(const SemanticItem& item)
		{
			SemanticInfo info;
			const std::string fullName(item.name);
			int i = static_cast<int>(fullName.length() - 1);
			std::string indexString;
			for (;i >= 0;--i)
			{
				if (fullName[i] >= '0' && fullName[i] <= '9')
					indexString += fullName[i];
				else
					break;
			}
			assert(indexString.length() < fullName.length() && "Semantic name error!A semantic name cannot be comprised of digits only!");
			if (indexString.length() > 0)
			{
				std::reverse(indexString.begin(), indexString.end());
				info.index = std::stoi(indexString);
			}
			else
			{
				info.index = 0;
			}
			info.rawName = fullName;
			info.name = fullName.substr(0, i + 1);
			return info;
		}

		void Renderer::GetSemanticInfo(RenderSemantics semantic, SemanticIndex& index, std::string& name)
		{
			auto it = mPipelineInputSemanticInfos.find(semantic);
			assert(it != mPipelineInputSemanticInfos.end() && "Invalid semantic!");
			index = it->second.index;
			name = it->second.name;
		}

		void Renderer::ApplyRenderPasses()
		{
			for (auto& pass : mRenderPasses)
			{
				pass->Apply(*mFrameResources[mFrameResourceIndex].renderQueue);
			}
		}

		void Renderer::Render()
		{
			if (!mStarted)
				return;
			WaitForPreviousFrame(false);
			mFrameCount++;
			mFrameResources[mFrameResourceIndex].OnFrameBegin();
			mFrameResources[mFrameResourceIndex].renderQueue = mCurrentFrameRenderQueue;
			if (mRenderQueueIndex == RENDER_FRAME_COUNT)
			{
				mRenderQueueIndex = 0;
			}
			else
			{
				++mRenderQueueIndex;
			}

			mCurrentFrameRenderQueue = &mRenderQueues[mRenderQueueIndex];
			OnFrameBegin();
			auto defaultRenderTarget = mSwapChain->GetDefaultRenderTarget();
			ClearRenderTarget(defaultRenderTarget, mClearColor);
			auto depthStencilBuffer = GetDefaultDepthStencilBuffer();
			ClearDepthStencilBuffer(depthStencilBuffer, DepthStencilClearFlags::CLEAR_DEPTH | DepthStencilClearFlags::CLEAR_STENCIL,
				depthStencilBuffer->GetDepthClearValue(), depthStencilBuffer->GetStencilClearValue(), nullptr);
			OnFrameUpdate();
			ApplyRenderPasses();
			OnFrameEnd();
			for (auto& pass : mRenderPasses)
			{
				pass->OnFrameEnd();
			}
			auto fence = mFrameResources[mFrameResourceIndex].fence;
			mFrameResources[mFrameResourceIndex].frame = mFrameCount;
			fence->SetTargetValue(mFrameCount);
			mSwapChain->Present();
			mFrameResourceIndex = mSwapChain->GetCurrentBackBufferIndex();
			g_RenderAllocator.FinishFrame(mFrameCount);
		}

		void Renderer::SetClearColor(float r, float g, float b, float a)
		{
			mClearColor = ColorF{r, g, b, a};
		}

		std::uint64_t Renderer::GetCurrentFrameCount()const
		{
			return mFrameCount;
		}

		IDevice* Renderer::GetDevice()
		{
			return mDevice.get();
		}

		ISwapChain* Renderer::GetSwapChain()
		{
			return mSwapChain.get();
		}

		IRenderUnit* Renderer::CreateRenderUnit()
		{
			return new (RenderUnitPool::malloc()) RenderUnit;
		}

		void Renderer::CommitRenderUnit(IRenderUnit* unit)
		{
			assert(unit != nullptr && "Commit render unit cannot be nullptr!");
			unit = unit->Clone();
			//unit->AddRef();
			mCurrentFrameRenderQueue->push_back(unit);
			for (auto& pass : mRenderPasses)
			{
				pass->OnAddRenderUnit(unit);
			}
		}

		void Renderer::AddRenderPass(RenderPassType type)
		{
			auto pass = CreateRenderPass(type);
			if(pass)
				mRenderPasses.emplace_back(pass);
		}

		std::size_t Renderer::GetFrameResourceIndex()const
		{
			return mFrameResourceIndex;
		}

		RenderPass* Renderer::CreateRenderPass(RenderPassType type)
		{
			switch (type)
			{
			case RenderPassType::FORWARD:
				return new ForwardRenderPass();
			case RenderPassType::DEFERED:
				return new DeferedRenderPass();
			default:
				break;
			}
			return nullptr;
		}

		void Renderer::Start()
		{
			if (mStarted)
				return;
			mDevice.reset(CreateDevice());
			mSwapChain.reset(CreateSwapChain());
			for (size_t i = 0; i < RENDER_FRAME_COUNT; i++)
			{
				mFrameResources[i].fence = CreateRenderFence();
				TextureDescriptor descriptor;
				descriptor.depth = 1;
				descriptor.width = mOutputWindow->GetWidth();
				descriptor.height = mOutputWindow->GetHeight();
				descriptor.multiSampleCount = mSwapChain->GetMultiSampleCount();
				descriptor.multiSampleQuality = mSwapChain->GetMultiSampleQuality();
				descriptor.numberOfMipmaps = 1;
				descriptor.type = TEXTURE_TYPE_2D;
				descriptor.format = RenderFormat::D24_S8;
				descriptor.depthClearValue = 1.0f;
				descriptor.stencilClearValue = 0;
				auto texture = mDevice->CreateTexture(descriptor, nullptr);
				mFrameResources[i].defaultDepthStencilBuffer = mDevice->CreateDepthStencilBuffer(texture);
				texture->Release();
			}
			mFrameResourceIndex = mSwapChain->GetCurrentBackBufferIndex();
			AddRenderPass(RenderPassType::FORWARD);
			mStarted = true;
		}

		void Renderer::ShutDown()
		{
			if (!mStarted)
				return;
			WaitForPreviousFrame(true);
			for (std::size_t i = 0;i < RENDER_FRAME_COUNT;++i)
			{
				mFrameResources[i].Release();
			}
			mFrameResources[0].renderQueue = mCurrentFrameRenderQueue;
			mFrameResources[0].ReleaseRenderQueue();
			mOutputWindow->Release();
			mDevice.reset();
			mSwapChain.reset();
			mRenderPasses.clear();
			mStarted = false;
		}

		IDepthStencilBuffer* Renderer::GetDefaultDepthStencilBuffer()
		{
			return mFrameResources[mFrameResourceIndex].defaultDepthStencilBuffer;
		}

		RenderSemantics Renderer::GetUniformSemantic(const char* uniform_name)
		{
			auto it = mUniformToSemantics.find(std::string(uniform_name));
			if (it != mUniformToSemantics.end())
			{
				return it->second;
			}
			return RenderSemantics::UNKNOWN;
		}

		const char* Renderer::GetUniformName(RenderSemantics semantic)
		{
			auto it = mSemanticsToUniform.find(semantic);
			if (it != mSemanticsToUniform.end())
			{
				return it->second;
			}
			return nullptr;
		}

		void Renderer::WaitForPreviousFrame(bool waitAll)
		{
			Container::Vector<UINT> bufferIndice;
			if (!waitAll)
			{
				bufferIndice.push_back(mSwapChain->GetCurrentBackBufferIndex());
			}
			else
			{
				for (std::uint32_t i = 0;i < RENDER_FRAME_COUNT;++i)
				{
					bufferIndice.push_back(i);
					//explicit signal to prevent release assert
					mFrameResources[i].fence->SetTargetValue(mFrameResources[i].fence->GetTargetValue() + 1);
				}
			}
			for (const auto& bufferIndex : bufferIndice)
			{
				auto& frameResource = mFrameResources[bufferIndex];
				frameResource.fence->WaitForTarget();
				g_RenderAllocator.ReleaseFramesBefore(frameResource.frame);
			}
		}
	}
}