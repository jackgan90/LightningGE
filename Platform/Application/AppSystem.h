#pragma once
#include <memory>
#include "PlatformExportDef.h"
#include "ECS/Entity.h"
#include "ECS/System.h"
#include "ECS/Event.h"
#include "FileSystem.h"
#include "TimerManager.h"
#include "AppComponent.h"

namespace Lightning
{
	namespace Render
	{
		class IRenderer;
	}
	namespace App
	{
		using UniqueRendererPtr = std::unique_ptr<Render::IRenderer>;

		using Foundation::Entity;
		using Foundation::EntityPtr;
		using Foundation::ComponentPtr;
		using Window::SharedWindowPtr;
		
		class LIGHTNING_PLATFORM_API AppSystem : public Foundation::System
		{
		public:
			AppSystem();
			virtual ~AppSystem();
			void Update(const EntityPtr& entity)override;
		protected:
			virtual void Start();
			virtual void OnWindowIdle(const Window::WindowIdleEvent& event);
			virtual SharedWindowPtr CreateMainWindow() = 0;
			virtual UniqueRendererPtr CreateRenderer() = 0;
			virtual void RegisterWindowHandlers();
			virtual void OnAppComponentRemoved(const ComponentPtr& component);
			//For test
			void GenerateSceneObjects();

			std::shared_ptr<AppComponent> mAppComponent;
		};

	}
}