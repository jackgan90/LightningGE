#pragma once
#include <memory>
#include "WindowEvents.h"
#include "IFileSystem.h"
#include "IWindow.h"
#include "IApplication.h"
#include "IRenderer.h"
#include "ECS/IEventManager.h"
#include "Container.h"

namespace Lightning
{
	namespace App
	{
		using Render::IRenderer;
		using Foundation::IFileSystem;
		using Window::IWindow;
		
		class Application : public IApplication
		{
		public:
			Application();
			~Application()override;
			void Update()override;
			void Start()override;
			bool IsRunning() override{ return mRunning; }
			int GetExitCode()const override{ return mExitCode; }
		protected:
			virtual void OnWindowIdle(const Foundation::IEvent& event);
			virtual void RegisterWindowHandlers();
			virtual void OnQuit(int exitCode);

			IFileSystem* mFileSystem;
			IRenderer* mRenderer;
			IWindow* mWindow;
			int mExitCode;
			bool mRunning;
			Foundation::Container::UnorderedSet<Foundation::EventSubscriberID> mSubscriberIDs;
		};

	}
}
