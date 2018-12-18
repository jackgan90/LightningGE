#pragma once
#include <memory>
#include <exception>
#include <functional>
#include "RenderException.h"
#include "IDevice.h"
#include "ISwapChain.h"
#include "IDepthStencilBuffer.h"
#include "IRenderTargetManager.h"
#include "Color.h"
#include "Math/Matrix.h"
#include "RenderNode.h"
#include "RenderPass.h"
#include "IWindow.h"

namespace Lightning
{
	namespace Render
	{
		class DeviceInitException : public RendererException
		{
		public:
			DeviceInitException(const char*const w):RendererException(w){}
		};

		class SwapChainInitException : public RendererException
		{
		public:
			SwapChainInitException(const char*const w):RendererException(w){}
		};

		enum class DrawType
		{
			Vertex,
			Index
		};

		struct DrawParam
		{
			DrawType drawType;
			union
			{
				std::size_t vertexCount;
				std::size_t indexCount;
			};
			std::size_t instanceCount;
			union
			{
				std::size_t firstVertex;
				std::size_t firstIndex;
			};
			//A value added to each index before reading a vertex from the vertex buffer.
			//only valid for index draw type.Vertex draw type will simply ignore this value
			std::size_t baseIndex;
			std::size_t baseInstance;
		};

		enum class RendererEvent
		{
			FRAME_BEGIN,
			FRAME_UPDATE,
			FRAME_POST_UPDATE,
			FRAME_END
		};

		using RendererCallback = std::function<void()>;
		class IRenderer
		{
		public:
			virtual ~IRenderer() = default;
			//entry point of render system
			virtual void Render() = 0;
			//return the device of render system
			virtual IDevice* GetDevice() = 0;
			virtual ISwapChain* GetSwapChain() = 0;
			virtual Window::IWindow* GetOutputWindow() = 0;
			//set default render target clear color.At the beginning of each frame,the back buffer is cleared to this color
			virtual void SetClearColor(float r, float g, float b, float a) = 0;
			//get the current frame index
			virtual std::uint64_t GetCurrentFrameCount()const = 0;
			virtual std::size_t GetFrameResourceIndex()const = 0;
			virtual void AddRenderPass(RenderPassType type) = 0;
			//Commit a render node for rendering
			virtual void CommitRenderNode(const RenderNode& node) = 0;
			//clear a specified render target,possibly parts of it defined by an array of rects
			virtual void ClearRenderTarget(IRenderTarget* renderTarget, const ColorF& color, const RectIList* rects=nullptr) = 0;
			virtual void ClearDepthStencilBuffer(IDepthStencilBuffer* buffer, DepthStencilClearFlags flags, float depth, std::uint8_t stencil, const RectIList* rects = nullptr) = 0;
			virtual void ApplyRenderTargets(const Container::Vector<IRenderTarget*>& renderTargets, IDepthStencilBuffer* dsBuffer) = 0;
			virtual void ApplyPipelineState(const PipelineState& state) = 0;
			//bind pBuffer to a GPU slot(does not copy data,just binding), each invocation will override previous binding
			virtual void BindGPUBuffer(std::uint8_t slot, IGPUBuffer* buffer) = 0;
			//issue underlying draw call
			virtual void Draw(const DrawParam& param) = 0;
			//register renderer event callback.The callback will be called on certain moment of rendering
			virtual void RegisterCallback(RendererEvent evt, RendererCallback cb) = 0;
			//get near plane value corresponding to normalized device coordinate
			//different render API may have different near plane definition
			//for example OpenGL clips coordinates to [-1, 1] and DirectX clips coordinates to [0, 1]
			//This method returns 0 for DirectX and -1 for OpenGL
			virtual float GetNDCNearPlane()const = 0;
			//start the renderer
			virtual void Start() = 0;
			//Shut down the renderer
			virtual void ShutDown() = 0;
			//get default depth stencil buffer
			virtual IDepthStencilBuffer* GetDefaultDepthStencilBuffer() = 0;
			//Gets the semantic corresponds to a string representing a uniform name
			virtual RenderSemantics GetUniformSemantic(const char* uniform_name) = 0;
			//Gets uniform name by RenderSemantics
			virtual const char* GetUniformName(RenderSemantics semantic) = 0;
		};
	}
}
