#pragma once
#include "ECS/Component.h"
#include "filesystem.h"
#include "irenderer.h"
#include "timer.h"

namespace Lightning
{
	namespace App
	{
		using Foundation::Component;
		using Foundation::SharedFileSystemPtr;
		using UniqueRendererPtr = std::unique_ptr<Render::IRenderer>;
		using Window::SharedWindowPtr;
		using Foundation::ITimer;

		struct AppComponent : Component
		{
			AppComponent():timer(nullptr){}
			SharedFileSystemPtr fileSystem;
			UniqueRendererPtr renderer;
			SharedWindowPtr window;
			ITimer *timer;
			long long exitCode;
			RTTR_ENABLE(Component)
		};
	}
}