#include <memory>
#include "PlatformPlugin.h"
#include "Application/ApplicationFactory.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"
#include "Logger.h"

namespace Lightning
{
	namespace Plugins
	{
		using App::ApplicationFactory;
		using App::IApplication;
		class PlatformPluginImpl : public PlatformPlugin
		{
		public:
			~PlatformPluginImpl()override;
			IApplication* CreateApplication()override;
			void Update()override;
		protected:
			void OnCreated(IPluginMgr*)override;
		private:
			std::unique_ptr<IApplication> mApp;
			IPluginMgr* mPluginMgr;
		};

		void PlatformPluginImpl::OnCreated(IPluginMgr* mgr)
		{
			mPluginMgr = mgr;
			INIT_LOGGER(mgr, Platform)
			LOG_INFO("Platform plugin init.");
		}

		PlatformPluginImpl::~PlatformPluginImpl()
		{
			LOG_INFO("Platform plugin unloaded.");
			FINALIZE_LOGGER(mPluginMgr)
		}

		void PlatformPluginImpl::Update()
		{
			if (mApp)
				mApp->Update();
		}

		IApplication* PlatformPluginImpl::CreateApplication()
		{
			mApp = ApplicationFactory::CreateApplication();
			return mApp.get();
		}
	}
}

LIGHTNING_PLUGIN_IMPL(PlatformPluginImpl)
