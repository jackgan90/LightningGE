#include <sstream>
#include <d3dx12.h>
#include "d3d12device.h"
#include "renderer.h"
#include "shadermanager.h"
#include "d3d12shader.h"

namespace LightningGE
{
	namespace Render
	{
		using Foundation::FileSize;
		using Foundation::FilePointerType;
		using Foundation::FileAnchor;

		D3D12Shader::D3D12Shader(ShaderType type, const std::string& name, const char* const shaderSource, const ComPtr<ID3D10Blob>& byteCode,
			int smMajor, int smMinor, const std::string& entry):
			Shader(entry), m_type(type)
			,m_name(name), m_byteCode(byteCode)
			,m_smMajorVersion(smMajor), m_smMinorVersion(smMinor)
#ifndef NDEBUG
			,m_source(shaderSource)
#endif
		{

		}

		D3D12Shader::~D3D12Shader()
		{

		}

		ShaderType D3D12Shader::GetType()const
		{
			return m_type;
		}

		const ShaderDefine D3D12Shader::GetMacros()const
		{
			return m_macros;
		}

		std::string D3D12Shader::GetName()const
		{
			return m_name;
		}

#ifndef NDEBUG
		const char* const D3D12Shader::GetSource()const
		{
			return m_source;
		}
#endif

		void D3D12Shader::GetShaderModelVersion(int& major, int& minor)
		{
			major = m_smMajorVersion;
			minor = m_smMinorVersion;
		}

		void* D3D12Shader::GetByteCodeBuffer()const
		{
			if (m_byteCode)
			{
				return m_byteCode->GetBufferPointer();
			}
			return nullptr;
		}

		SIZE_T D3D12Shader::GetByteCodeBufferSize()const
		{
			if (m_byteCode)
			{
				return m_byteCode->GetBufferSize();
			}
			return 0;
		}

	}
}