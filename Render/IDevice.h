#pragma once
#include <memory>
#include <functional>
#include "Container.h"
#include "RendererExportDef.h"
#include "IRenderTarget.h"
#include "IDepthStencilBuffer.h"
#include "PipelineState.h"
#include "IGPUBuffer.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "IShader.h"
#include "Texture/Itexture.h"
#include "Rect.h"
#include "Color.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		using ShaderLoadFinishHandler = std::function<void(const SharedShaderPtr&)>;
		using TextureLoadFinishHandler = std::function<void(const SharedTexturePtr&)>;
		class IDevice
		{
		public:
			virtual ~IDevice() = default;
			virtual SharedVertexBufferPtr CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual SharedIndexBufferPtr CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual SharedShaderPtr CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual void CreateShaderFromFile(ShaderType type, const std::string& path,
				const ShaderDefine& defineMap, ShaderLoadFinishHandler handler) = 0;
			virtual SharedTexturePtr CreateTexture(const TextureDescriptor& descriptor, char* buffer) = 0;
			virtual void CreateTextureFromFile(const TextureDescriptor& descriptor, const std::string& path,
				TextureLoadFinishHandler handler) = 0;
			virtual SharedShaderPtr GetDefaultShader(ShaderType type) = 0;
		};
		using SharedDevicePtr = std::shared_ptr<IDevice>;
	}
}