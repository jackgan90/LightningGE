#include <algorithm>
#include "PluginMgr.h"

namespace Lightning
{
	namespace Plugins
	{
		PluginMgr::PluginMgr():mNeedSyncPlugins(false), mPluginUpdatePriority(1024)
		{

		}

		PluginMgr::~PluginMgr()
		{
			SynchronizeTables();
		}

		void PluginMgr::MakePlugin1UpdateBeforePlugin2(Plugin* plugin1, Plugin* plugin2)
		{
			assert(plugin1 && plugin2 && "Both plugin1 and plugin2 must be valid!");
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			plugin1->SetUpdateOrder(plugin2->GetUpdateOrder() - 1);
			mNeedSyncPlugins = true;
		}

		void PluginMgr::Update()
		{
			for (auto& pluginInfo : mPluginsToUpdate)
			{
				pluginInfo.plugin->Update();
			}

			SynchronizeTables();
		}

		void PluginMgr::SynchronizeTables()
		{
			if (mNeedSyncPlugins)
			{
				std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
				while (!mOperationQueue.empty())
				{
					auto operation = mOperationQueue.front();
					mOperationQueue.pop_front();
					switch (operation.type)
					{
					case OperationType::Load:
						mPlugins[operation.name] = mPendingAddPlugins[operation.name];
						break;
					case OperationType::Unload:
						UnloadPlugin(mPlugins, operation.name);
						break;
					default:
						break;
					}
				}
				mPendingAddPlugins.clear();
				mPluginsToUpdate.clear();
				for (auto it = mPlugins.begin(); it != mPlugins.end();++it)
				{
					mPluginsToUpdate.push_back(it->second);
				}
				std::sort(mPluginsToUpdate.begin(), mPluginsToUpdate.end(), 
					[this](const PluginInfo& info1, const PluginInfo& info2) {
					return ComparePluginUpdateOrder(info1, info2);
				});
				mNeedSyncPlugins = false;
			}
		}

		bool PluginMgr::ComparePluginUpdateOrder(const PluginInfo& info1, const PluginInfo& info2)
		{
			return info1.plugin->GetUpdateOrder() < info2.plugin->GetUpdateOrder();
		}

		Plugin* PluginMgr::LoadPlugin(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);

			auto plugin = LookUpPlugin(mPendingAddPlugins, name, true);
			if (plugin)
				return plugin;

			plugin = LookUpPlugin(mPlugins, name, true);
			if (plugin)
				return plugin;

			PluginInfo info;
#ifdef LIGHTNING_WIN32
			info.handle = ::LoadLibrary((name + Plugin::PluginExtension).c_str());
			if (info.handle)
			{
				GetPluginProc pGetProc = (GetPluginProc)::GetProcAddress(info.handle, GET_PLUGIN_PROC);
				if (pGetProc)
				{
					info.plugin = pGetProc(this);
					if (info.plugin)
					{
						info.plugin->SetName(name);
						info.plugin->SetUpdateOrder(mPluginUpdatePriority.fetch_add(1, std::memory_order_relaxed));
						info.plugin->OnCreated(this);
						//mPlugins.emplace(name, info);
						mPendingAddPlugins.emplace(name, info);
						Operation operation;
						operation.info = info;
						operation.name = name;
						operation.type = OperationType::Load;
						mOperationQueue.push_back(operation);
						//Needs to be the last statement.So that main thread will get data from operation queue.
						mNeedSyncPlugins = true;
						return info.plugin;
					}
				}
				::FreeLibrary(info.handle);
			}
#endif
			return nullptr;
		}

		Plugin* PluginMgr::LookUpPlugin(PluginTable& table, const std::string& name, bool addRef)
		{
			if (table.empty())
				return nullptr;
			auto it = table.find(name);
			if (it != table.end())
			{
				if (addRef)
					it->second.plugin->AddRef();
				return it->second.plugin;
			}
			return nullptr;
		}

		Plugin* PluginMgr::GetPlugin(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);

			auto plugin = LookUpPlugin(mPendingAddPlugins, name, false);
			if (plugin)
				return plugin;

			plugin = LookUpPlugin(mPlugins, name, false);
			if (plugin)
				return plugin;

			return nullptr;
		}

		bool PluginMgr::UnloadPlugin(PluginTable& table, const std::string& name)
		{
			auto it = table.find(name);
			if (it != table.end())
			{
				if (it->second.plugin->Release())
				{
#ifdef LIGHTNING_WIN32
					::FreeLibrary(it->second.handle);
#endif
					table.erase(it);
				}
				return true;
			}
			return false;
		}

		bool PluginMgr::UnloadPlugin(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> lock(mPluginsMutex);
			auto res = UnloadPlugin(mPendingAddPlugins, name);
			if (res)
				return true;

			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
			{
				Operation operation;
				operation.info = it->second;
				operation.name = name;
				operation.type = OperationType::Unload;
				mOperationQueue.push_back(operation);
				mNeedSyncPlugins = true;
				return true;
			}
			return false;
		}
	}
}