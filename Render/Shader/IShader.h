#pragma once
#include <string>
#include <exception>
#include <boost/functional/hash.hpp>
#include "Semantics.h"
#include "IShaderMacros.h"
#include "Parameter.h"
#undef DOMAIN

namespace Lightning
{
	namespace Render
	{
		enum class ShaderType : std::uint8_t
		{
			VERTEX = 1,
			FRAGMENT,
			GEOMETRY,
			HULL, //hull shader
			DOMAIN,	//domain shader
		};

		struct IShader
		{
			virtual ~IShader() = default;
			virtual ShaderType GetType()const = 0;
			virtual void DefineMacro(const std::string& macroName, const std::string& macroValue) = 0;
			virtual std::shared_ptr<IShaderMacros> GetMacros()const = 0;
			virtual std::size_t GetParameterCount()const = 0;
			virtual void Compile() = 0;
			virtual std::string GetName()const = 0;
			virtual bool SetParameter(const Parameter& parameter) = 0;
			virtual ParameterType GetParameterType(const std::string& name)const = 0;
			virtual std::string GetSource()const = 0;
			virtual void GetUniformSemantics(RenderSemantics** semantics, std::uint16_t& semanticCount) = 0;
			virtual std::size_t GetHash()const = 0;
		};
	}
}

