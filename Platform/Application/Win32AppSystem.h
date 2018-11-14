#pragma once
#include "PlatformExportDef.h"
#include "AppSystem.h"
#include "WindowSystem.h"
#include "FileSystem.h"

namespace Lightning
{
	namespace App
	{
		using Window::SharedWindowPtr;
		class LIGHTNING_PLATFORM_API Win32AppSystem : public AppSystem
		{
		public:
			Win32AppSystem();
			~Win32AppSystem()override;
			void Update(const EntityPtr& entity)override;
		protected:
			void OnMouseWheel(const Window::MouseWheelEvent& event);
			void OnKeyDown(const Window::KeyEvent& event);
			void OnMouseDown(const Window::MouseDownEvent& event);
			void OnMouseMove(const Window::MouseMoveEvent& event);
			void RegisterWindowHandlers()override;
			SharedWindowPtr CreateMainWindow()override;
			UniqueRendererPtr CreateRenderer()override;
		};
	}
}