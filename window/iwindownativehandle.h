#pragma once
#include "windowexportdef.h"
#include <memory>

namespace LightningGE
{
	namespace WindowSystem
	{
		enum NativeHandleIdentifier
		{
			IDENTIFIER_WINDOWS_HANDLE,
		};
		class LIGHTNINGGE_WINDOW_API IWindowNativeHandle
		{
		public:
			virtual bool operator==(const IWindowNativeHandle& handle) = 0;
			virtual ~IWindowNativeHandle(){}
			virtual NativeHandleIdentifier GetIdentifier()const = 0;
		};
		using WindowNativeHandlePtr = std::shared_ptr<IWindowNativeHandle>;
	}
}