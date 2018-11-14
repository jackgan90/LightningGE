#pragma once
#include <cstdint>
#include "EnumOperation.h"

namespace Lightning
{
	namespace Render
	{
		enum class RenderFormat : std::uint8_t
		{
			UNDEFINED,
			R32G32B32_FLOAT,
			R32G32B32A32_FLOAT,
			R8G8B8A8_UNORM,
			D24_S8,		//for depth-stencil buffer
			//TODO : there're more formats to add here...
		};

		enum class PrimitiveType : std::uint8_t
		{
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP
		};

		enum class DepthStencilClearFlags : std::uint8_t
		{
			CLEAR_DEPTH = 0x01,
			CLEAR_STENCIL = 0x02,
		};
		ENABLE_ENUM_BITMASK_OPERATORS(DepthStencilClearFlags)

		constexpr int DEFAULT_SHADER_MODEL_MAJOR_VERSION = 5;
		constexpr int DEFAULT_SHADER_MODEL_MINOR_VERSION = 0;
		constexpr int MAX_GEOMETRY_BUFFER_COUNT = 8;
		constexpr int RENDER_FRAME_COUNT = 3;
		constexpr int MAX_RENDER_TARGET_COUNT = 8;
		constexpr char* const DEFAULT_SHADER_ENTRY = "main";
	}
}