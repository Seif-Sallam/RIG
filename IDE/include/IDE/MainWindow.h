#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

#include <imgui.h>
#include <imguial_term.h>
#include <ImGuiFileDialog.h>

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
		void InitializeWindow();
		void SetUpSettings();
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

		void OpenDialog(const std::string& key, const std::string& title, const char* filters, const std::string& filePathName, 
		std::function<void(const std::string&, const std::string&)> delegate, int countSelectionMax = 1, IGFD::UserDatas userData = (IGFD::UserDatas)nullptr, ImGuiFileDialogFlags flags = 0);
		void UpdateDialogs();
		void SetWorkSpaceDir(const std::string& dir);
	private:
		int32_t m_Width;
		int32_t m_Height;

		sf::RenderWindow* m_Window;
		TextEditor m_TextEditor;
		std::vector<TextEditor> m_Tabs;

		std::unordered_map<std::string, std::unordered_map<uint32_t, ImFont*>> m_Fonts;
		std::string m_ActiveFont;
		int32_t m_ActiveFontSize;

		const std::string m_TextEditorWindowName;
		const std::string m_RegisterFileWindowName;
		const std::string m_LogWindowName;
		std::string m_WorkSpaceDir;
		std::string m_ChosenPalette;

		bool m_LayoutInitialized;
		bool m_ConfigOpened;
		bool m_IsMaximized;

		std::vector<std::pair<std::string, void(*)(MainWindow*)>> m_Settings;

		std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>> m_DialogToUpdate;

	};


}