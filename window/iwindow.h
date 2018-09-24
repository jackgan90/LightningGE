#pragma once
#include <memory>
#include <unordered_map>
#include <exception>
#include "windowexportdef.h"
#include "windowmessage.h"
#include "logger.h"

#define WINDOW_MSG_CLASS_HANDLER(pWindow, MessageType, MessageParamType, Handler)\
(pWindow)->RegisterWindowMessageHandler((MessageType), [&](WindowMessage, const WindowMessageParam& param)\
{this->Handler(static_cast<const MessageParamType&>(param)); })

namespace Lightning
{
	namespace WindowSystem
	{
		class WindowInitException : public std::exception
		{
		public:
			template<typename... Args>
			WindowInitException(const char*const w, const Args&... args) : exception(w)
			{
				LOG_ERROR(w, args...);
			}
		};
		class LIGHTNING_WINDOW_API IWindow
		{
		public:
			friend class WindowManager;
			virtual bool Show(bool show) = 0;
			virtual void RegisterWindowMessageHandler(WindowMessage msg, WindowMessageHandler handler) = 0;
			virtual void PostWindowMessage(WindowMessage msg, const WindowMessageParam& param) = 0;
			virtual std::uint32_t GetWidth()const = 0;
			virtual std::uint32_t GetHeight()const = 0;
			virtual int GetDestroyCode() = 0;
			virtual ~IWindow() = default;
		protected:
			IWindow() {};
		};
		using SharedWindowPtr = std::shared_ptr<IWindow>;
	}
}
