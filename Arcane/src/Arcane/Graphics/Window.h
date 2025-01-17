#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#ifndef APPLICATION_H
#include <Arcane/Core/Application.h>
#endif

namespace Arcane
{
	class Window
	{
	public:
		Window(Application *application, const ApplicationSpecification &specification);
		~Window();

		void Init();

		/**
		* Will swap the screen buffers and will poll all window/input events
		*/
		void Update();
		bool Closed() const;
		static void ClearAll();
		static void ClearColour();
		static void ClearDepth();
		static void ClearStencil();
		static void Bind();

		inline GLFWwindow* GetNativeWindow() { return m_Window; }
		static inline bool GetHideCursor() { return s_HideCursor; }
		static inline bool GetHideUI() { return s_HideUI; }
		static inline int GetWidth() { return s_Width; }
		static inline int GetHeight() { return s_Height; }
		static inline int GetRenderResolutionWidth() { return s_RenderResolutionWidth; }
		static inline int GetRenderResolutionHeight() { return s_RenderResolutionHeight; }
	private:
		bool InitInternal();
		void SetFullscreenResolution();

#ifdef ARC_DEV_BUILD
		static friend void UpdateUIState(GLFWwindow *window, int key, int scancode, int action, int mods);
#endif // ARC_DEV_BUILD

		// Callback Functions
		static friend void error_callback(int error, const char* description);
		static friend void window_close_callback(GLFWwindow *window);
		static friend void window_resize_callback(GLFWwindow *window, int width, int height);
		static friend void framebuffer_resize_callback(GLFWwindow *window, int width, int height);
		static friend void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
		static friend void key_callback_imgui(GLFWwindow *window, int key, int scancode, int action, int mods);
		static friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		static friend void mouse_button_callback_imgui(GLFWwindow* window, int button, int action, int mods);
		static friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
		static friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static friend void scroll_callback_imgui(GLFWwindow* window, double xoffset, double yoffset);
		static friend void char_callback_imgui(GLFWwindow* window, unsigned int c);
		static friend void joystick_callback(int joystick, int event);
		static friend void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
	private:
		Application *m_Application;
		const char *m_Title;
		GLFWwindow *m_Window;

		static bool s_HideCursor;
		static bool s_HideUI;
		static int s_Width, s_Height;
		static int s_RenderResolutionWidth, s_RenderResolutionHeight;
		static bool s_VSync;

		static bool s_EnableImGui;
	};
}
#endif
