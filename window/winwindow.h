#pragma once
#include "iwindow.h"
#include "winwindownativehandle.h"
#include <string>

namespace LightningGE
{
	namespace WindowSystem
	{
		class LIGHTNINGGE_WINDOW_API WinWindow : public IWindow
		{
		public:
			WinWindow();
			void destroy()override;
			~WinWindow()override;
			bool Init()override;
			bool Show(bool show)override;
			const IWindowNativeHandle* GetNativeHandle()const override;
			WINDOWWIDTH GetWidth()const override;
			WINDOWHEIGHT GetHeight()const override;
		private:
			friend LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			friend WinWindow* GetWinWindow(HWND hWnd);
			WindowNativeHandlePtr m_nativeHandle;
			std::string m_Caption;
			WINDOWWIDTH m_width;
			WINDOWHEIGHT m_height;
			static char* s_WindowClassName;
		};
	}
}