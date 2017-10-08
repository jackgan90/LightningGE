#pragma once
#include <memory>
#include "platformexportdef.h"
#include "iwindow.h"
#include "memorysystem.h"

namespace LightningGE
{
	namespace App
	{
		template<typename AllocatorType, typename className>
		class LIGHTNINGGE_PLATFORM_API IApplication : public memory::ILeakFreeObject<AllocatorType, className>
		{
		public:
			virtual bool Init() = 0;
			virtual bool Start() = 0;
			virtual int Run() = 0;
			virtual void Quit() = 0;
			virtual ~IApplication(){}
		};

		template<typename AllocatorType, typename className>
		using ApplicationPtr = std::shared_ptr<IApplication<AllocatorType, className>>;

		template<typename AllocatorType, typename className>
		class LIGHTNINGGE_PLATFORM_API Application : public IApplication<AllocatorType, className>
		{
		public:
			bool Start()override;
		protected:
			virtual void OnWindowIdle(const WindowSystem::WindowIdleParam& param){}
			virtual bool CreateMainWindow() = 0;
			virtual bool InitRenderContext() = 0;
			virtual void RegisterWindowHandlers();
			virtual WindowSystem::WindowPtr GetMainWindow() const = 0;
		};

#ifdef	LIGHTNINGGE_PLATFORM_EXPORT
		template<typename AllocatorType, typename className>
		bool Application<AllocatorType, className>::Start()
		{
			bool result = true;
			result &= CreateMainWindow();
			if(result)
				result &= InitRenderContext();
			if (result)
				RegisterWindowHandlers();
			return result;
		}

		template<typename AllocatorType, typename className>
		void Application<AllocatorType, className>::RegisterWindowHandlers()
		{
			WindowPtr pWin = GetMainWindow();
			if (pWin)
			{
				pWin->RegisterWindowMessageHandler(WindowSystem::MESSAGE_IDLE, 
					[&](WindowSystem::WindowMessage msg, const WindowSystem::WindowMessageParam& param) 
					{this->OnWindowIdle(reinterpret_cast<const WindowSystem::WindowIdleParam&>(param)); });
			}
		}

#endif
	}
}
