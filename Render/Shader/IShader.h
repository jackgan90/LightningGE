#pragma once
#include <string>
#include <exception>
#include <boost/functional/hash.hpp>
#include "ShaderMacros.h"
#include "HashableObject.h"
#include "Semantics.h"
#include "RenderException.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "RefCount.h"
#include "Texture/ITexture.h"
#include "Texture/Sampler.h"

namespace Lightning
{
	namespace Render
	{
		using Foundation::Math::Vector2f;
		using Foundation::Math::Vector3f;
		using Foundation::Math::Vector4f;
		using Foundation::Math::Matrix3f;
		using Foundation::Math::Matrix4f;
		class ShaderCompileException : public RendererException
		{
		public:
			ShaderCompileException(const char*const w):RendererException(w){}
		};

		using Utility::HashableObject;
		enum class ShaderType
		{
			VERTEX = 1,
			FRAGMENT,
			GEOMETRY,
			TESSELATION_CONTROL, //hull shader
			TESSELATION_EVALUATION	//domain shader
		};

		enum class ShaderParameterType
		{
			UNKNOWN,
			FLOAT,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			MATRIX2,
			MATRIX3,
			MATRIX4,
			TEXTURE,
			SAMPLER,
		};
		using ShaderParameterRegister = std::uint8_t;
		using ShaderParameterSpace = std::uint8_t;

		struct ShaderParameter
		{
			ShaderParameterType type;
			ShaderParameterRegister registerIndex;
			ShaderParameterSpace registerSpace;
			std::string name;
			union
			{
				float f;
				Vector2f v2;
				Vector3f v3;
				Vector4f v4;
				Matrix3f m3;
				Matrix4f m4;
				ITexture* texture;
				SamplerState samplerState;
			};
			ShaderParameter():type(ShaderParameterType::UNKNOWN){}
			ShaderParameter(const std::string& n, const float _f):name(n), type(ShaderParameterType::FLOAT), f(_f){}
			ShaderParameter(const std::string& n, const Vector2f& _v2) :name(n), type(ShaderParameterType::FLOAT2) { new (&v2)Vector2f(_v2); }
			ShaderParameter(const std::string& n, const Vector3f& _v3) :name(n), type(ShaderParameterType::FLOAT3) { new (&v3)Vector3f(_v3); }
			ShaderParameter(const std::string& n, const Vector4f& _v4) :name(n), type(ShaderParameterType::FLOAT4) { new (&v4)Vector4f(_v4); }
			ShaderParameter(const std::string& n, const Matrix3f& _m3) :name(n), type(ShaderParameterType::MATRIX3) { new (&m3)Matrix3f(_m3); }
			ShaderParameter(const std::string& n, const Matrix4f& _m4) :name(n), type(ShaderParameterType::MATRIX4) { new (&m4)Matrix4f(_m4); }
			ShaderParameter(const std::string& n, ITexture* _texture) :name(n), type(ShaderParameterType::TEXTURE) { texture = _texture; }
			ShaderParameter(const std::string& n, const SamplerState& _state) :name(n), type(ShaderParameterType::SAMPLER) { samplerState = _state; }
			const void* Buffer(std::size_t& size)const
			{
				size = 0;
				switch (type)
				{
				case ShaderParameterType::UNKNOWN:
					break;
				case ShaderParameterType::FLOAT:
					size = sizeof(f);
					return &f;
				case ShaderParameterType::FLOAT2:
					size = sizeof(v2);
					return &v2;
				case ShaderParameterType::FLOAT3:
					size = sizeof(v3);
					return &v3;
				case ShaderParameterType::FLOAT4:
					size = sizeof(v4);
					return &v4;
				case ShaderParameterType::MATRIX2:
					break;
				case ShaderParameterType::MATRIX3:
					size = sizeof(m3);
					return &m3;
				case ShaderParameterType::MATRIX4:
					size = sizeof(m4);
					return &m4;
				case ShaderParameterType::TEXTURE:
					return texture;
				case ShaderParameterType::SAMPLER:
					return &samplerState;
				default:
					break;
				}
				return nullptr;
			}
		};

		class IShader : public HashableObject, public Plugins::RefCount
		{
		public:
			virtual std::string GetEntryPoint()const = 0; 
			virtual void SetEntryPoint(const std::string& entryPoint) = 0;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacros(const ShaderMacros& macros) = 0;
			virtual const ShaderMacros& GetMacros()const = 0;
			virtual std::size_t GetParameterCount()const = 0;
			virtual void Compile() = 0;
			virtual std::string GetName()const = 0;
			virtual bool SetParameter(const ShaderParameter& parameter) = 0;
			virtual const char* const GetSource()const = 0;
			virtual void GetShaderModelVersion(int& major, int& minor) = 0;
			virtual void GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount) = 0;
		};

		class Shader : public IShader
		{
		public:
			static size_t Hash(const ShaderType& type, const std::string& shaderName, const ShaderMacros& macros);
			Shader(ShaderType type, const std::string& name, const std::string& entryPoint, const char* const source);
			~Shader()override;
			void SetEntryPoint(const std::string& entryPoint)override;
			void DefineMacros(const ShaderMacros& define)override;
			std::string GetEntryPoint()const override { return mEntryPoint; }
			ShaderType GetType()const override;
			const char* const GetSource()const override;
			std::string GetName()const override;
			void GetShaderModelVersion(int& major, int& minor)override;
		protected:
			size_t CalculateHashInternal()override;
			ShaderType mType;
			std::string mName;
			std::string mEntryPoint;
			const char* const mSource;
			ShaderMacros mMacros;
			int mShaderModelMajorVersion;
			int mShaderModelMinorVersion;
		};
	}
}

namespace std
{
	template<> struct hash<Lightning::Render::IShader>
	{
		std::size_t operator()(const Lightning::Render::IShader& shader)const noexcept
		{
			return Lightning::Render::Shader::Hash(shader.GetType(), shader.GetName(), shader.GetMacros());
		}
	};
}
