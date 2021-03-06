#pragma once
#include <string>
#include <functional>
#include "boost/variant.hpp"
#include "PipelineState.h"
#include "IShader.h"
#include "Parameter.h"

namespace Lightning
{
	namespace Render
	{
		struct IMaterial
		{
			virtual ~IMaterial() = default;
			//set shader used by this material
			virtual void SetShader(ShaderType shaderType, const std::shared_ptr<IShader>& shader) = 0;
			virtual std::shared_ptr<IShader> GetShader(ShaderType shaderType) = 0;
			virtual bool SetParameter(const Parameter& parameter) = 0;
			template<typename ValueType>
			bool SetParameter(const std::string& name, ValueType&& value)
			{
				return SetParameter(Parameter(name, std::forward<ValueType>(value)));
			}
			//Get a parameter named name,if the specified parameter with the given name exists,returns true, otherwise returns false.
			virtual bool GetParameter(const std::string& name, Parameter& parameter)const = 0;
			//Gets number of parameters of a specific type
			virtual std::size_t GetParameterTypeCount(ParameterType parameterType)const = 0;
			virtual void EnableBlend(bool enable) = 0;
			virtual void GetBlendState(BlendState& blendState)const = 0;
			//The visitor should not modify the underlying parameter map during iteration
			virtual void VisitParameters(std::function<void(const Parameter& parameter)> visitor)const = 0;
		};
	}
}