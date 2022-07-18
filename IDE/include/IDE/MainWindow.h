#pragma once

#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <imgui.h>
#include <imguial_term.h>
#include <TextEditor.h>

namespace IDE
{
	class MainWindow
	{
	public:
		MainWindow(uint32_t argc, const char *argv[]);
		void Run();
		~MainWindow();

		inline static void SetWidth(int32_t value) { m_Width = value; }
		inline static void SetHeight(int32_t value) { m_Height = value; }

	private:
		void HandleEvents();
		void ImGuiLayer();
		void Update();
		void Render();

		void InitilizeWindow();
		void SetupLayout(ImGuiID &dockSpaceID);
		void SetupDockspace();

		ImVec2 DockspaceMenuBar();

		void LogImGui();
		void RegisterFileImGui();
		void TextEditorImGui();

	private:
		static int32_t m_Width;
		static int32_t m_Height;
		GLFWwindow *m_Window;
		TextEditor m_TextEditor;
		ImGuiID m_DockID;
		ImGuiID m_DockIDRight;
		ImGuiID m_DockIDLeft;
		ImGuiID m_DockIDBottom;
		std::string m_TextEditorWindowName;
		std::string m_RegisterFileWindowName;
		std::string m_LogWindowName;

		bool m_LayoutInitialized = false;
	};

}