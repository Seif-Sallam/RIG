#pragma once

#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <imgui.h>
#include <imguial_term.h>
#include "IDE/TextEditor.h"

namespace IDE
{
	class MainWindow
	{
	public:
		MainWindow(uint32_t argc, const char *argv[]);
		void Run();
		~MainWindow();

		inline void SetWidth(int32_t value) { m_Width = value; }
		inline void SetHeight(int32_t value) { m_Height = value; }

	private:

		// The main loop
		void HandleEvents();
		void ImGuiLayer();
		void Update();
		void Render();


		// Setting up
		void InitilizeWindow();
		void AddDefaultFonts();
		void SetupLayout(ImGuiID dockSpaceID);
		void SetupDockspace();

		// State
		void LoadState();
		void SaveState();

		void SaveDocument(const TextEditor& textEditor, const std::string& filePath);

		// GUI
		ImVec2 DockspaceMenuBar();
		void LogWindow();
		void RegisterFileWindow();
		void TextEditorWindow();
		void SettingsWindow();
		void ConfigWindow();

	private:
		int32_t m_Width;
		int32_t m_Height;

		GLFWwindow *m_Window;
		TextEditor m_TextEditor;
		std::vector<TextEditor> m_Tabs;

		std::unordered_map<std::string, std::unordered_map<uint32_t, ImFont*>> m_Fonts;
		std::string m_ActiveFont;
		int32_t m_ActiveFontSize;

		const std::string m_TextEditorWindowName;
		const std::string m_RegisterFileWindowName;
		const std::string m_LogWindowName;

		bool m_LayoutInitialized;
		bool m_ConfigOpened;
	};


}