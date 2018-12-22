#pragma once
#include <functional>
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "IShader.h"
#include "Texture/Itexture.h"
#include "ISerializeBuffer.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Container;
		using Loading::ISerializeBuffer;
		//Avoid using template in cross module interfaces
		struct IShaderCallback
		{
			virtual ~IShaderCallback() = default;
			virtual void Execute(IShader*) = 0;
		};

		struct ITextureCallback
		{
			virtual ~ITextureCallback() = default;
			virtual void Execute(ITexture*) = 0;
		};

		struct IDevice
		{
			virtual ~IDevice() = default;
			virtual IVertexBuffer* CreateVertexBuffer(std::uint32_t bufferSize, const VertexDescriptor& descriptor) = 0;
			virtual IIndexBuffer* CreateIndexBuffer(std::uint32_t bufferSize, IndexType type) = 0;
			virtual IShader* CreateShader(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderMacros& macros) = 0;
			virtual void CreateShaderFromFile(ShaderType type, const char* const path, IShaderCallback* callback) = 0;
			virtual ITexture* CreateTexture(const TextureDescriptor& descriptor, ISerializeBuffer* buffer) = 0;
			virtual void CreateTextureFromFile(const char* const path, ITextureCallback* callback) = 0;
			virtual IShader* GetDefaultShader(ShaderType type) = 0;
		};
	}
}
