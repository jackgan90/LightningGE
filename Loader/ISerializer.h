#pragma once
#include "IFileSystem.h"
#include "ISerializeBuffer.h"


namespace Lightning
{
	namespace Loading
	{
		enum LoadType
		{
			LOAD_TYPE_TEXTURE = 0,
			LOAD_TYPE_SHADER,
			LOAD_TYPE_NUM
		};

		struct ISerializer
		{
			virtual ~ISerializer() = default;
			virtual void Serialize(char** buffer) = 0;
			virtual void Deserialize(Foundation::IFile* file, ISerializeBuffer* buffer) = 0;
			//The Dispose method is the last method called by Loader.Subclass of it should do clean up work(recyle memory)
			virtual void Dispose() = 0;
		};
	}
}