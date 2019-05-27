#pragma once
#include <cstdint>
#include "ISpaceCamera.h"
#include "ISpaceObject.h"

namespace Lightning
{
	namespace World
	{
		using Render::ICamera;
		struct IScene : virtual ISpaceObject
		{
			virtual std::uint32_t GetID()const = 0;
			virtual void Tick() = 0;
			virtual ISpaceCamera* GetActiveCamera() = 0;
		};
	}
}