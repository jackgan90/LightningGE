#pragma once

namespace LightningGE
{
	namespace Render
	{
		using SemanticIndex = unsigned int;
		enum class RenderSemantics
		{
			POSITION,
			NORMAL,
			TANGENT,
			CAMERA_POSITION,
			FRAME_COUNT,
			FRAME_DELTA_TIME,
			LIGHT_POSITION
		};

		struct SemanticItem
		{
			RenderSemantics semantic;
			char name[32];
		};

		const SemanticItem EngineSemantics[] = {
			{RenderSemantics::POSITION, "POSITION"},
			{RenderSemantics::NORMAL, "NORMAL"},
			{RenderSemantics::TANGENT, "TANGENT"},
			{RenderSemantics::CAMERA_POSITION, "CAMERA_POSITION"},
			{RenderSemantics::FRAME_COUNT, "FRAME_COUNT"},
			{RenderSemantics::FRAME_DELTA_TIME, "FRAME_DELTA_TIME"},
			{RenderSemantics::LIGHT_POSITION, "LIGHT_POSITION"},
		};
	}
}