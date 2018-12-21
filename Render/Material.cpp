#include "Material.h"

namespace Lightning
{
	namespace Render
	{
		Material::Material()
		{

		}

		Material::~Material()
		{
			for (auto& pair : mShaders)
			{
				pair.second.shader->Release();
			}
		}

		IShader* Material::GetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it == mShaders.end())
				return nullptr;
			return it->second.shader;
		}

		void Material::GetShaders(Container::Vector<IShader*>& shaders)
		{
			auto vs = GetShader(ShaderType::VERTEX);
			if (vs)
				shaders.push_back(vs);
			auto fs = GetShader(ShaderType::FRAGMENT);
			if (fs)
				shaders.push_back(fs);
			auto gs = GetShader(ShaderType::GEOMETRY);
			if (gs)
				shaders.push_back(gs);
			auto tcs = GetShader(ShaderType::TESSELATION_CONTROL);
			if (tcs)
				shaders.push_back(tcs);
			auto tes = GetShader(ShaderType::TESSELATION_EVALUATION);
			if (tes)
				shaders.push_back(tes);
		}

		void Material::SetShader(IShader* shader)
		{
			if (!shader)
				return;
			shader->AddRef();
			ResetShader(shader->GetType());
			mShaders[shader->GetType()].shader = shader;
		}

		bool Material::ResetShader(ShaderType type)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				it->second.shader->Release();
				mShaders.erase(it);
				return true;
			}
			return false;
		}

		bool Material::SetParameter(ShaderType type, const ShaderParameter& arg)
		{
			auto it = mShaders.find(type);
			if (it != mShaders.end())
			{
				it->second.parameters.push_back(arg);
				return true;
			}
			return false;
		}

		void Material::EnableBlend(bool enable)
		{
			mBlendState.enable = enable;
			mBlendState.alphaOp = BlendOperation::ADD;
			mBlendState.colorOp = BlendOperation::ADD;
			mBlendState.srcColorFactor = BlendFactor::SRC_ALPHA;
			mBlendState.srcAlphaFactor = BlendFactor::SRC_ALPHA;
			mBlendState.destColorFactor = BlendFactor::INV_SRC_ALPHA;
			mBlendState.destAlphaFactor = BlendFactor::INV_SRC_ALPHA;
		}
	}
}