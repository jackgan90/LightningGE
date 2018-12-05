#include "Loader.h"
#include "Logger.h"
#include "tbb/task.h"
#include "IPluginMgr.h"
#include "FoundationPlugin.h"
#include "SerializeBuffer.h"

namespace Lightning
{
	namespace Plugins
	{
		extern IPluginMgr* gPluginMgr;
	}
	namespace Loading
	{
		DeserializeTask::DeserializeTask(const LoadTask& loadTask, Foundation::IFile* file, 
			ISerializeBuffer* buffer, bool ownBuffer)
			:mLoadTask(loadTask), mFile(file), mBuffer(buffer), mOwnFile(ownBuffer)
		{
			mFile->AddRef();
			mBuffer->AddRef();
		}

		DeserializeTask::~DeserializeTask()
		{
			mBuffer->Release();
			mFile->Release();
		}


		tbb::task* DeserializeTask::execute()
		{
			mLoadTask.serializer->Deserialize(mFile, mBuffer);
			mLoadTask.serializer->Dispose();
			if (mOwnFile)
			{
				Loader::Instance()->DisposeFile(mLoadTask.path, mFile);
			}
			return nullptr;
		}

		Loader::Loader() : mRunning(true), mLoaderIOThread(IOThread)
		{
		}

		Loader::~Loader()
		{
		}

		void Loader::Finalize()
		{
			mRunning = false;
			mCondVar.notify_one();
			mLoaderIOThread.join();
		}

		void Loader::Load(const std::string& path, ISerializer* ser)
		{
			LoadTask task;
			task.serializer = ser;
			task.path = path;
			mTasks.push(task);
			mCondVar.notify_one();
		}

		void Loader::DisposeFile(const std::string& path, Foundation::IFile* file)
		{
			mDisposedPathes.push(path);
			file->Close();
			mCondVar.notify_one();
		}

		//This thread only deals with IO related stuff.After finishing reading a file
		//it will hand over the buffer and file to a tbb task so that deserialization 
		//happens in tbb threads.
		void Loader::IOThread()
		{
			auto foundation = Plugins::gPluginMgr->GetPlugin<Plugins::FoundationPlugin>("Foundation");
			//If Users has exited app,foundation will be null
			if (!foundation)
				return;
			auto mgr = Loader::Instance();
			auto fileSystem = foundation->GetFileSystem();
			auto environment = foundation->GetEnvironment();
			environment->SetLoaderIOThreadID(std::this_thread::get_id());
			LOG_INFO("LoaderMgr IO Thread start!");
			while (mgr->mRunning)
			{
				std::string path;
				while (mgr->mDisposedPathes.try_pop(path))
				{
					auto it = mgr->mBuffers.find(path);
					if (it != mgr->mBuffers.end())
					{
						it->second->Release();
						mgr->mBuffers.erase(it);
					}
				}
				LoadTask task;
				if (mgr->mTasks.try_pop(task))
				{
					LOG_INFO("Start to load file : {0}", task.path);
					auto file = fileSystem->FindFile(task.path, Foundation::FileAccess::READ);
					if (!file)
					{
						LOG_ERROR("Can't find file : {0}", task.path.c_str());
						continue;
					}
					if (file->IsOpen())
					{
						auto deserializeTask = new (tbb::task::allocate_root()) DeserializeTask(task, file, mgr->mBuffers[task.path], false);
						tbb::task::enqueue(*deserializeTask);
						continue;
					}
					auto size = file->GetSize();
					if (size <= 0)
					{
						LOG_ERROR("File is empty : {0}", file->GetPath().c_str());
						file->Close();
						continue;
					}
					file->SetFilePointer(Foundation::FilePointerType::Read, Foundation::FileAnchor::Begin, 0);
					//auto sharedBuffer = std::shared_ptr<char>(new char[std::size_t(size + 1)], std::default_delete<char[]>());
					auto sharedBuffer = NEW_REF_OBJ(SerializeBuffer, size);
					auto buffer = sharedBuffer->GetBuffer();
					buffer[size] = 0;
					auto readSize = file->Read(buffer, size);
					if (readSize < size)
					{
						LOG_ERROR("Unable to read whole file : {0}", file->GetPath());
						file->Close();
						sharedBuffer->Release();
						continue;
					}
					mgr->mBuffers[task.path] = sharedBuffer;
					LOG_INFO("Loader finished reading file : {0}, buffer size : {1}", task.path, size);
					auto deserializeTask = new (tbb::task::allocate_root()) DeserializeTask(task, file, sharedBuffer, true);
					tbb::task::enqueue(*deserializeTask);
				}
				else
				{
					std::unique_lock<std::mutex> lock(mgr->mTaskQueueMutex);
					mgr->mCondVar.wait(lock);
				}
			}
			LOG_INFO("Loader IO Thread exit!");
		}

	}
}