#include "ecs/entitymanager.h"
#include "ecs/systemmanager.h"
#include "ecs/eventmanager.h"
#include "AppComponent.h"
#ifdef LIGHTNING_WIN32
#include <Windows.h>	//for WinMain
#include "Win32AppSystem.h"
#endif
#include "logger.h" //for logging

using Lightning::Foundation::EntityManager;
using Lightning::Foundation::EventManager;
using Lightning::Foundation::SystemManager;
using Lightning::Foundation::ComponentPtr;
using Lightning::Foundation::Entity;
using Lightning::Foundation::Component;
using Lightning::App::AppComponent;
namespace
{
	bool running{ true };
}
#ifdef LIGHTNING_WIN32
using Lightning::App::Win32AppSystem;
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
#endif
{
	LOG_INFO("This is Lighting entry.");
	auto appEntity = EntityManager::Instance()->CreateEntity<Entity>();
	auto appComponent = appEntity->AddComponent<AppComponent>();
	int exitCode{ 0 };
	Lightning::Foundation::EntityFuncID id = 
	appEntity->RegisterCompRemovedFunc<AppComponent>([&](const std::shared_ptr<AppComponent>& comp) {
		exitCode = static_cast<int>(comp->exitCode);
		running = false;
		appEntity->UnregisterCompRemovedFunc(id);
	});
#ifdef LIGHTNING_WIN32
	auto app = SystemManager::Instance()->CreateSystem<Win32AppSystem>();
#endif
	while (running)
	{
		EntityManager::Instance()->Update();
	}
	return exitCode;
}
