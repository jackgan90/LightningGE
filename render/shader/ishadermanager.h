#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "ishader.h"

namespace Lightning
{
	namespace Render
	{
		class IShaderManager
		{
		public:
			//return a compiled shader ptr if the specified shader can be created successfully
			virtual ~IShaderManager() = default;
			virtual SharedShaderPtr GetShader(size_t shaderHash) = 0;
			virtual SharedShaderPtr CreateShaderFromSource(ShaderType type, const std::string& shaderName, const char* const shaderSource, const ShaderDefine& defineMap) = 0;
			virtual SharedShaderPtr CreateShaderFromFile(ShaderType type, const std::string& fileName, const ShaderDefine& defineMap) = 0;
		protected:
			IShaderManager(){}
		};
	}
}