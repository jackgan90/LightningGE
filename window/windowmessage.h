#pragma once
#include <functional>
#include "iwindow.h"

namespace Lightning
{
	namespace WindowSystem
	{
		class IWindow;
		enum class WindowMessage
		{
			CREATED,
			DESTROYED,
			IDLE,
			RESIZE,
			MOUSE_WHEEL,
			KEY_DOWN,
		};

		enum VirtualKeyCode
		{
			//alphabet
			VK_A = 0,
			VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L, VK_M, VK_N,
			VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z
		};

		struct WindowMessageParam
		{
			WindowMessageParam(const IWindow* ptr):pWindow(ptr){}
			const IWindow* pWindow;
		};

		struct WindowCreatedParam : WindowMessageParam
		{
		};

		struct WindowDestroyedParam : WindowMessageParam
		{
		};

		struct WindowIdleParam : WindowMessageParam
		{
			WindowIdleParam(const IWindow* ptr) : WindowMessageParam(ptr){}
		};

		struct WindowResizeParam : WindowMessageParam
		{
			WindowResizeParam(const IWindow* ptr) : WindowMessageParam(ptr){}
			unsigned int width;
			unsigned int height;
		};

		struct MouseWheelParam : WindowMessageParam
		{
			MouseWheelParam(const IWindow* ptr) : WindowMessageParam(ptr){}
			int wheel_delta;
			bool is_vertical;
		};

		struct KeyParam : WindowMessageParam
		{
			KeyParam(const IWindow* ptr) : WindowMessageParam(ptr){}
			VirtualKeyCode code;
		};

		using WindowMessageHandler = std::function<void(WindowMessage, const WindowMessageParam&)>;

	}
}
