#pragma once
#include <functional>
#include "iwindow.h"
#include "ecs/event.h"

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
			MOUSE_DOWN,
			MOUSE_MOVE,
		};

		enum VirtualKeyCode
		{
			//alphabet
			VK_A = 1,
			VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L, VK_M, VK_N,
			VK_O, VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z,
			//special keys
			VK_CONTROL_MASK = 0xffffff00,
			VK_CTRL = 0x100,
			VK_MOUSELBUTTON = 0x200,
			VK_MOUSEMBUTTON = 0x400,
			VK_MOUSERBUTTON = 0x800,
			VK_SHIFTBUTTON = 0x1000,
		};

		template<typename Derived>
		struct WindowEvent : Foundation::Event<Derived>
		{
			WindowEvent(const IWindow* ptr) : pWindow(ptr){}
			const IWindow* pWindow;
		};

		struct WindowCreatedEvent : WindowEvent<WindowCreatedEvent>
		{
			WindowCreatedEvent(const IWindow* ptr) : WindowEvent<WindowCreatedEvent>(ptr){}
		};

		struct WindowDestroyedEvent : WindowEvent<WindowDestroyedEvent>
		{
			WindowDestroyedEvent(const IWindow* ptr) : WindowEvent<WindowDestroyedEvent>(ptr){}
		};

		struct WindowIdleEvent : WindowEvent<WindowIdleEvent>
		{
			WindowIdleEvent(const IWindow* ptr) : WindowEvent<WindowIdleEvent>(ptr){}
		};

		struct WindowResizeEvent : WindowEvent<WindowResizeEvent>
		{
			WindowResizeEvent(const IWindow* ptr) : WindowEvent<WindowResizeEvent>(ptr){}
			unsigned int width;
			unsigned int height;
		};

		struct MouseWheelEvent : WindowEvent<MouseWheelEvent>
		{
			MouseWheelEvent(const IWindow* ptr) : WindowEvent<MouseWheelEvent>(ptr){}
			int wheel_delta;
			bool is_vertical;
		};

		struct KeyEvent : WindowEvent<KeyEvent>
		{
			KeyEvent(const IWindow* ptr) : WindowEvent<KeyEvent>(ptr){}
			VirtualKeyCode code;
		};

		struct MouseDownEvent : WindowEvent<MouseDownEvent>
		{
			MouseDownEvent(const IWindow* ptr) : WindowEvent<MouseDownEvent>(ptr){}
			std::size_t x;
			std::size_t y;
			VirtualKeyCode pressedKey;
		};

		struct MouseMoveEvent : WindowEvent<MouseMoveEvent>
		{
			MouseMoveEvent(const IWindow* ptr) : WindowEvent<MouseMoveEvent>(ptr){}
			std::size_t x;
			std::size_t y;
			VirtualKeyCode pressedKey;
		};
	}
}
