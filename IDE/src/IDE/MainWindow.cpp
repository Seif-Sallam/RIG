#include <imgui.h>

#include "IDE/MainWindow.h"

#include <imgui-SFML.h>
#include <imguial_term.h>
#include <imguial_button.h>
#include <imgui_internal.h>

#include <fstream>
#include <functional>

#include <stdio.h>

#include <Utils/Logger.h>
#include <Utils/Functions.h>

#include <filesystem>
#include <ImGuiFileDialog.h>

namespace IDE
{

#pragma region Static Variables

	using Log = Util::Logger;
	static MainWindow* s_Instance = nullptr;

	static std::vector<std::string> paletteNames= {
			"Default"
			,"Keyword"
			,"Number"
			,"String"
			,"Char literal"
			,"Punctuation"
			,"Preprocessor"
			,"Identifier"
			,"Known identifier"
			,"Preproc identifier"
			,"Comment (single line)"
			,"Comment (multi line)"
			,"Background"
			,"Cursor"
			,"Selection"
			,"ErrorMarker"
			,"Breakpoint"
			,"Breakpoint outline"
			,"Current line indicator"
			,"Current line indicator outline"
			,"Line number"
			,"Current line fill"
			,"Current line fill (inactive)"
			,"Current line edge"
			,"Error message"
			,"BreakpointDisabled"
			,"UserFunction"
			,"UserType"
			,"UniformType"
			,"GlobalVariable"
			,"LocalVariable"
			,"FunctionArgument"
		};
#pragma endregion

#pragma region Constructor and Destructors
	MainWindow::MainWindow(uint32_t argc, const char *argv[])
		: m_Width(800)
		, m_Height(600)
		, m_TextEditorWindowName("Text Editors")
		, m_LogWindowName("Log")
		, m_RegisterFileWindowName("Register File")
		, m_ConfigOpened(false)
		, m_LayoutInitialized(false)
		, m_IsMaximized(true)
		, m_ActiveFontSize(24)
		, m_ActiveFont("Consolos")
		, m_WorkSpaceDir(RESOURCES_DIR"/")
	{
		s_Instance = this;

		InitializeWindow();

		m_TextEditor.SetFunctionTooltips(true);
		m_TextEditor.SetShowWhitespaces(true);
		m_TextEditor.SetColorizerEnable(true);
		m_TextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::RISCV());

		this->AddDefaultFonts();

		LoadState();
		SetUpSettings();
		m_TextEditor.SetText(".data\n\n.text\n");
		m_TextEditor.SetCurrentOpenedPath(RESOURCES_DIR"/Untitled");

		Log::Warning("{}", m_ChosenPalette);
		ImGui::SFML::Init(*m_Window);

	}

	MainWindow::~MainWindow()
	{
		SaveState();
		delete m_Window;
		ImGui::DestroyContext();
	}

	void MainWindow::InitializeWindow()
	{
		sf::VideoMode mode = sf::VideoMode::getFullscreenModes()[0];
		m_Height = mode.height;
		m_Width = mode.width;

		m_Window = new sf::RenderWindow(sf::VideoMode(m_Width, m_Height), "RIG, v0.0.2", sf::Style::Default);

		// sf::RenderWindow win()
		if (!m_Window)
		{
			fprintf(stderr, "Failed to open GLFW window.\n");
			exit(-1);
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();
	}

	void MainWindow::SetUpSettings()
	{
		m_Settings.push_back(std::make_pair(
			"Font", [](IDE::MainWindow* ide)
			{
				Util::BeginGroupPanel("Font");
				static std::string selectedFont = ide->m_ActiveFont;
				static int32_t pixelSize = ide->m_ActiveFontSize;
				// Begin a combo box that contains the options for the fonts
				uint32_t currentFontSize = ide->m_ActiveFontSize;
				ImGui::Text("Active Font: %s, Size: %u", ide->m_ActiveFont.c_str(), currentFontSize);
				if (ImGui::BeginCombo("###1", selectedFont.c_str()))
				{
					for (auto& entry : ide->m_Fonts)
					{
						if (ImGui::RadioButton(entry.first.c_str(), selectedFont == entry.first))
						{
							selectedFont = entry.first;
						}
					}
					// End the combo box provided we opened it
					ImGui::EndCombo();
				}
				ImGui::TextUnformatted("Pixel Size: ");
				ImGui::SameLine();
				ImGui::SliderInt("###pixel_size_choser", &pixelSize, 18, 48);
				pixelSize = (pixelSize % 2) ? pixelSize + 1 : pixelSize;
				bool activeButton = selectedFont != ide->m_ActiveFont || pixelSize != currentFontSize;
				if (ImGuiAl::Button("Change Font", activeButton))
				{
					ide->m_ActiveFont = selectedFont;
					ide->m_ActiveFontSize = pixelSize;
					selectedFont = ide->m_ActiveFont;
					pixelSize = ide->m_ActiveFontSize;
				}
				Util::EndGroupPanel();
			}
		));

		m_Settings.push_back(std::make_pair(
			"Test Panal",
			[](MainWindow* ide)
			{
				Util::BeginGroupPanel("Test Panal");
				ImGui::TextUnformatted("hello, world second thing");
				Util::EndGroupPanel();
			}
		));

		m_Settings.push_back(std::make_pair(
			"Text Editor Palette",
			[](MainWindow* ide)
			{
				Util::BeginGroupPanel("Text Editor Palette");
				static int currentItem = 0;
				static int lastItemChosen = 0;
				ImGui::Text("Active Palette: %s", ide->m_ChosenPalette.c_str());
				ImGui::Combo("##palleteChoiceCombo", &currentItem, "Dark Palette\0Light Palette\0Retro Blue Palette\0");
				if (lastItemChosen != currentItem)
				{
					// static IDE::TextEditor::Palette palettes[3] = {TextEditor::GetDarkPalette(), TextEditor::GetLightPalette(), TextEditor::GetRetroBluePalette()};
					lastItemChosen = currentItem;
					switch (currentItem)
					{
						case 0:
							ide->m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
							ide->m_ChosenPalette = "Dark Palette";
						break;
						case 1:
							ide->m_TextEditor.SetPalette(TextEditor::GetLightPalette());
							ide->m_ChosenPalette = "Light Palette";
						break;
						case 2:
							ide->m_TextEditor.SetPalette(TextEditor::GetRetroBluePalette());
							ide->m_ChosenPalette = "Retro Blue Palette";
						break;
						default:
							ide->m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
							ide->m_ChosenPalette = "Dark Palette";
					}
					// ide->m_TextEditor.SetPalette(palettes[currentItem]);

				}
				static bool openedEditor = false;
				if (ImGuiAl::Button("Open Palette Editor##1", !openedEditor))
					openedEditor = true;
				Util::EndGroupPanel();
				if (openedEditor)
				{
					static 	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
												   | ImGuiWindowFlags_AlwaysAutoResize;
					ImGui::Begin("Palette Editor###2", &openedEditor, flags);
					{
						static TextEditor::Palette p = {
							0xffff0000,	// "Default"
							0xffd69c56,	// "Keyword"
							0xff00ff00,	// "Number"
							0xff7070e0,	// "String"
							0xff70a0e0, // "Char literal"
							0xffffffff, // "Punctuation"
							0xff408080,	// "Preprocessor"
							0xffaaaaaa, // "Identifier"
							0xff9bc64d, // "Known identifier"
							0xffc040a0, // "Preproc identifier"
							0xff206020, // "Comment (single line)"
							0xff406020, // "Comment (multi line)"
							0xff101010, // "Background"
							0xffe0e0e0, // "Cursor"
							0x80a06020, // "Selection"
							0x800020ff, // "ErrorMarker"
							0xff0000ff, // "Breakpoint"
							0xffffffff, // "Breakpoint outline"
							0xFF1DD8FF, // "Current line indicator"
							0xFF696969, // "Current line indicator outline"
							0xff707000, // "Line number"
							0x40000000, // "Current line fill"
							0x40808080, // "Current line fill (inactive)"
							0x40a0a0a0, // "Current line edge"
							0xff33ffff, // "Error message"
							0xffffffff, // "BreakpointDisabled"
							0xffaaaaaa, // "UserFunction"
							0xffb0c94e, // "UserType"
							0xffaaaaaa, // "UniformType"
							0xffaaaaaa, // "GlobalVariable"
							0xffaaaaaa, // "LocalVariable"
							0xff888888	// "FunctionArgument"
						};
						static ImColor cols[32];
						static bool init = false;
						if(!init)
						{
							for(int i =0 ;i < 32; i++)
								cols[i] = p[i];

							init = true;
						}

						for(size_t i = 0; i < p.size(); i++)
						{
							std::string title = paletteNames[i] + "###" +  std::to_string(i);
							ImGui::ColorEdit3(title.c_str(), &cols[i].Value.x);

							uint32_t color = cols[i];
							p[i] = color;
						}
						ide->m_TextEditor.SetPalette(p);
						ide->m_ChosenPalette = "Custom";
					}
					ImGui::End();
				}
			}
		));
	}
#pragma endregion

#pragma region Run_Functions

	void MainWindow::Run()
	{
		sf::Clock clk;
		while (m_Window->isOpen())
		{
			sf::Time time = clk.restart();
			HandleEvents();

			Update();
			ImGui::SFML::Update(*m_Window, time);

			ImGui::PushFont(m_Fonts[m_ActiveFont][m_ActiveFontSize]);
			{
				SetupDockspace();
				ImGuiLayer();
			}
			ImGui::PopFont();

			Render();
		}
	}

	void MainWindow::HandleEvents()
	{
		sf::Event event;
		while(m_Window->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			switch(event.type)
			{
				case sf::Event::Closed:
					m_Window->close();
				break;
			}
		}
	}

	void MainWindow::ImGuiLayer()
	{
		ConfigWindow();
		RegisterFileWindow();
		LogWindow();
		TextEditorWindow();
		UpdateDialogs();
	}

	void MainWindow::Update()
	{

	}

	void MainWindow::Render()
	{
		const sf::Color clearClr(68, 68, 68, 255);
		m_Window->clear(clearClr);
		ImGui::SFML::Render(*m_Window);
		m_Window->display();
	}

#pragma endregion

#pragma region Setup

	void MainWindow::SetupLayout(ImGuiID dockSpaceID)
	{
		m_LayoutInitialized = true;
		ImGui::DockBuilderRemoveNode(dockSpaceID);							   // Clear out existing layout
		ImGui::DockBuilderAddNode(dockSpaceID, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(dockSpaceID, ImVec2((float)m_Width, (float)m_Height));

		auto dockSpaceIDLeft = ImGui::DockBuilderSplitNode(dockSpaceID, ImGuiDir_Left, 0.20f, NULL  , &dockSpaceID);
		auto dockSpaceIDRight = ImGui::DockBuilderSplitNode(dockSpaceID, ImGuiDir_Right, 0.30f, NULL, &dockSpaceID);
		auto dockSpaceIDBottom = ImGui::DockBuilderSplitNode(dockSpaceID, ImGuiDir_Down, 0.40f, NULL, &dockSpaceID);

		ImGui::DockBuilderDockWindow(m_TextEditorWindowName.c_str(), dockSpaceID);
		ImGui::DockBuilderDockWindow(m_LogWindowName.c_str(), dockSpaceIDBottom);
		ImGui::DockBuilderDockWindow(m_RegisterFileWindowName.c_str(), dockSpaceIDRight);
		ImGui::DockBuilderFinish(dockSpaceID);
	}

	void MainWindow::SetupDockspace()
	{
		static const char *dockSpaceTitle = "Dockspace";
		static auto dockSpaceID = ImGui::GetID(dockSpaceTitle);
		{
			ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_None 					 | ImGuiWindowFlags_MenuBar
													| ImGuiWindowFlags_NoDocking  			 | ImGuiWindowFlags_NoTitleBar
													| ImGuiWindowFlags_NoCollapse 			 | ImGuiWindowFlags_NoScrollbar
													| ImGuiWindowFlags_NoMove 				 | ImGuiWindowFlags_NoNavFocus
													| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::SetNextWindowSize(ImVec2((float)m_Width, (float)m_Height));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::Begin(dockSpaceTitle, (bool *)nullptr, dockSpaceWindowFlags);
			auto menuSize = DockspaceMenuBar();

			ImVec2 windowSize((float)m_Width, (float)m_Height);
			auto size = ImVec2(windowSize);
			size.y = windowSize.y - menuSize.y - 2.0f;
			ImGui::DockSpace(dockSpaceID, size, ImGuiDockNodeFlags_NoWindowMenuButton);

			if (!m_LayoutInitialized)
				SetupLayout(dockSpaceID);

			ImGui::End();
			ImGui::PopStyleVar(3);
		}

	}

	ImVec2 MainWindow::DockspaceMenuBar()
	{
		ImGui::BeginMenuBar();
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open File"))
				{
					auto function = [&](const std::string& filePathName, const std::string& filePath)
					{
						Log::Info("Reading the file: {}", filePathName);
						if (!this->m_TextEditor.ReadFile(filePathName))
							Log::Error("Could not open the file: {}", filePathName);
						else
						{
							Log::Success("Read the file: {}", filePathName);
							SetWorkSpaceDir(filePath);
						}
					};

					this->OpenDialog("LoadFileDialog", "Load a new file", ".asm,.s,.S,.ASM", m_WorkSpaceDir, function,
					1, nullptr, ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideResetButton);
				}
				if (ImGui::MenuItem("Reset Layout"))
				{
					m_LayoutInitialized = false;
				}
				if (ImGui::MenuItem("Save"))
				{
					// auto textToSave = m_TextEditor.GetText();
					/// save text....
					SaveDocument(m_TextEditor, "C:/Users/User/Desktop/OutputFile.txt");
				}
				if (ImGui::MenuItem("Config"))
					m_ConfigOpened = !m_ConfigOpened;
				if (ImGui::MenuItem("Quit", "Alt+F4"))
				{
					// We should close :D
					m_Window->close();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				bool ro = m_TextEditor.IsReadOnly();
				if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
					m_TextEditor.SetReadOnly(ro);
				ImGui::Separator();

				if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && m_TextEditor.CanUndo()))
					m_TextEditor.Undo();
				if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && m_TextEditor.CanRedo()))
					m_TextEditor.Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, m_TextEditor.HasSelection()))
					m_TextEditor.Copy();
				if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && m_TextEditor.HasSelection()))
					m_TextEditor.Cut();
				if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && m_TextEditor.HasSelection()))
					m_TextEditor.Delete();
				if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
					m_TextEditor.Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select all", nullptr, nullptr))
					m_TextEditor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_TextEditor.GetTotalLines(), 0));

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				bool currentValue = m_IsMaximized;
				ImGui::Checkbox("Max screen", &m_IsMaximized);
				if (currentValue != m_IsMaximized)
				{
					uint32_t width = m_Window->getSize().x;
					uint32_t height = m_Window->getSize().y;
					auto settings = m_Window->getSettings();
			
					switch(currentValue)
					{
						case false:
							m_Window->close();
							m_Window->create(sf::VideoMode(m_Width, m_Height), "Rig v0.0.2", sf::Style::Fullscreen, settings);
						break;
						case true:
							m_Window->close();
							m_Window->create(sf::VideoMode(width, height), "Rig v0.0.2", sf::Style::Default, settings);
					}
				}
				if (ImGui::MenuItem("Dark palette"))
					m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light palette"))
					m_TextEditor.SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro blue palette"))
					m_TextEditor.SetPalette(TextEditor::GetRetroBluePalette());
				ImGui::EndMenu();
			}
		}
		ImVec2 size = ImGui::GetItemRectSize();
		ImGui::EndMenuBar();
		return size;
	}

	inline static void RemoveTTForOTF(std::string& str)
	{
		size_t index = str.find(".ttf");
		if (index == std::string::npos)
		{
			index = str.find(".otf");
			if(index = std::string::npos)
				return;
		}
		str.erase(str.begin() + index, str.end());
	}

	inline static ImFont* AddFont(const char* path, uint32_t pixelSize)
	{
		auto font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path, (float)pixelSize);
		Log::Info("Loaded font: {}, Size: {}", path, pixelSize);
		return font;
	}

	void MainWindow::AddDefaultFonts()
	{
		m_Fonts.clear();
		namespace fs = std::filesystem;
		fs::path path(RESOURCES_DIR"/fonts/");
		{
			auto& consolosFont = m_Fonts[m_ActiveFont];
			consolosFont[m_ActiveFontSize] = AddFont(RESOURCES_DIR"/fonts/Consolos.ttf", 24);
		}
		for (auto entry : fs::directory_iterator(path))
		{
			std::string fileName = entry.path().filename().string();
			RemoveTTForOTF(fileName);
			Log::Info("Adding font: {}", fileName);
			for(uint32_t fontSize = 18; fontSize <= 48; fontSize+=2)
				m_Fonts[fileName][fontSize] = AddFont(entry.path().string().c_str(), fontSize);
		}
	}

#pragma endregion

#pragma region States
	static const std::string g_SaveFile = RESOURCES_DIR"/ini/Config.ini";
	struct ParseData{
		std::string type;
		std::string data;
	};

	inline static ParseData ParseLine(const std::string& line)
	{
		size_t colonIndex = line.find(":");
		if(colonIndex == std::string::npos)
			return {};

		return { line.substr(0, colonIndex),
				 line.substr(colonIndex + 2) };
	}

	void MainWindow::LoadState()
	{
		std::ifstream inputFile(g_SaveFile, std::ios::binary);
		if (inputFile.is_open() == false)
		{
			Log::Warning("Failed to open config file, it will be created and written to it and deafult values will be loaded");
			return;
		}
		while(!inputFile.eof())
		{
			std::string line;
			std::getline(inputFile, line);
			if (line.size() < 3) break;
			ParseData parsedLine = ParseLine(line);
			Log::Info("Config: {}", line);
			if (parsedLine.type == "Font")
			{
				m_ActiveFont = parsedLine.data;
				bool foundFont = false;
				for (auto& font : m_Fonts)
				{
					if (font.first == m_ActiveFont)
					{
						foundFont = true;
						break;
					}
				}
				if (!foundFont)
				{
					m_ActiveFont = (*(m_Fonts.begin())).first;
					Log::Error("Invalid font in Config file. Loaded font: {}", m_ActiveFont);
				}
			}
			else if (parsedLine.type == "FontSize")
			{
				m_ActiveFontSize = std::stoi(parsedLine.data);
				m_ActiveFontSize = std::clamp(m_ActiveFontSize, 18, 48);
				m_ActiveFontSize = (m_ActiveFontSize % 2) ? m_ActiveFontSize + 1: m_ActiveFontSize;
			}
			else if (parsedLine.type == "PaletteData")
			{
				if (Util::IsInteger(parsedLine.data.c_str()))
				{
					uint32_t size = stoi(parsedLine.data);
					IDE::TextEditor::Palette p;
					for (uint32_t i = 0; i < size; i++)
					{
						std::getline(inputFile, line);
						size_t index = line.find(":");
						line = line.substr(index + 2);
						p[i] = Util::GetInteger(line.c_str(), (uint32_t)line.size());
					}
					m_TextEditor.SetPalette(p);
					m_ChosenPalette = "Custom";
				}
				else
				{
					if (parsedLine.data == "Dark Palette")
						m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
					else if (parsedLine.data == "Light Palette")
						m_TextEditor.SetPalette(TextEditor::GetLightPalette());
					else if (parsedLine.data == "Retro Blue Palette")
						m_TextEditor.SetPalette(TextEditor::GetRetroBluePalette());
					m_ChosenPalette = parsedLine.data;
				}
			}
		}
	}

	void MainWindow::SaveState()
	{
		std::ofstream outputFile(g_SaveFile, std::ios::binary);
		if (outputFile.is_open() == false)
		{
			Log::Warning("Failed to save config file.");
			return;
		}
		outputFile << "Font: " << m_ActiveFont << '\n';
		outputFile << "FontSize: " << m_ActiveFontSize << '\n';
		outputFile << "PaletteData: ";
		if (m_ChosenPalette == "Custom")
		{
			auto palette = m_TextEditor.GetPalette();
			outputFile << palette.size() << '\n';
			for (size_t i = 0; i < palette.size(); i++)
				outputFile << '\t' << paletteNames[i] << " : 0x" << std::setfill('0') << std::setw(8) << std::hex << palette[i] << '\n';
			std::cout << std::setfill(' ');
		}
		else
		{
			outputFile << m_ChosenPalette << '\n';
		}
		Log::Success("Saved data to the config file: {}", g_SaveFile);
	}

	void MainWindow::SaveDocument(const TextEditor& editor, const std::string& filePath)
	{
		std::ofstream outputFile;
		outputFile.open(filePath);
		if(outputFile.is_open())
		{
			outputFile << editor.GetText();
			Log::Success("File Saved Successfully, {}", filePath);
		}
		else
		{
			Log::Error("File was not saved, {}", filePath);
		}
	}

#pragma endregion

#pragma region Helpers
	// Fills lps[] for given patttern pat[0..M-1]
	inline static void computeLPSArray(char* pat, int M, int* lps)
	{
		// length of the previous longest prefix suffix
		int len = 0;

		lps[0] = 0; // lps[0] is always 0

		// the loop calculates lps[i] for i = 1 to M-1
		int i = 1;
		while (i < M) {
			if (pat[i] == pat[len]) {
				len++;
				lps[i] = len;
				i++;
			}
			else // (pat[i] != pat[len])
			{
				// This is tricky. Consider the example.
				// AAACAAAA and i = 7. The idea is similar
				// to search step.
				if (len != 0) {
					len = lps[len - 1];

					// Also, note that we do not increment
					// i here
				}
				else // if (len == 0)
				{
					lps[i] = 0;
					i++;
				}
			}
		}
	}


	// Prints occurrences of txt[] in pat[]
	inline static bool KMPSearch(char* pat, char* txt)
	{
		int M = (int)strlen(pat);
		int N = (int)strlen(txt);

		// create lps[] that will hold the longest prefix suffix
		// values for pattern
		// int lps[M];
		int lps[256];
		// Preprocess the pattern (calculate lps[] array)
		computeLPSArray(pat, M, lps);

		int i = 0; // index for txt[]
		int j = 0; // index for pat[]
		while ((N - i) >= (M - j)) {
			if (pat[j] == txt[i]) {
				j++;
				i++;
			}

			if (j == M) {
				return true;
				j = lps[j - 1];
			}

			// mismatch after j matches
			else if (i < N && pat[j] != txt[i]) {
				// Do not match lps[0..lps[j-1]] characters,
				// they will match anyway
				if (j != 0)
					j = lps[j - 1];
				else
					i = i + 1;
			}
		}
		return false;
	}

#pragma endregion

#pragma region Logic

	void MainWindow::ConfigWindow()
	{
		if(m_ConfigOpened)
		{
			ImGuiWindowFlags configsWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
												  | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("Config", &m_ConfigOpened, configsWindowFlags);
			{
				static std::string filter(256, '\0');
				static std::string filterCopy(256, '\0');
				ImGui::TextUnformatted("Filter");
				ImGui::SameLine();
				ImGui::InputText("###inputFilter", filter.data(), 255);
				filterCopy = filter;
				for(auto& c : filterCopy)
					c = tolower(c);
				for(auto& s : m_Settings)
				{
					std::string nameCpy = s.first;
					for(auto& c : nameCpy)
						c = tolower(c);
					if (KMPSearch(filterCopy.data(), nameCpy.data()))
						s.second(this);
				}
			}
			ImGui::End();

		}
	}

	void MainWindow::LogWindow()
	{
		Log::Draw(m_LogWindowName.c_str(), nullptr, ImGuiWindowFlags_None);
	}

	void MainWindow::RegisterFileWindow()
	{
		ImGui::Begin(m_RegisterFileWindowName.c_str());
		{
			ImGui::TextUnformatted("Register File Window Text");
		}
		ImGui::End();
	}

	void MainWindow::TextEditorWindow()
	{
		auto cpos = m_TextEditor.GetCursorPosition();
		// uint32_t i = 0;
		// for(auto& editor : m_Tabs)
		{
			ImGui::SetNextWindowDockID(ImGui::GetID("Dockspace"), ImGuiCond_Always);
			// i++;

			std::string title = Util::RemovePrefix(m_WorkSpaceDir, m_TextEditor.GetCurrentOpenedPath());

			ImGui::Begin(title.c_str() , nullptr, ImGuiWindowFlags_HorizontalScrollbar);
			{
				ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s",
							cpos.mLine + 1, cpos.mColumn + 1,
							m_TextEditor.GetTotalLines(),
							m_TextEditor.IsOverwrite() ? "Ovr" : "Ins",
							m_TextEditor.CanUndo() ? "*" : " ",
							m_TextEditor.GetLanguageDefinition().mName.c_str(), m_TextEditor.GetCurrentOpenedPath().c_str());
				// ImGui::PushFont(m_Fonts[m_ActiveFontIndex]);
				m_TextEditor.Render("TextEditor");
				// ImGui::PopFont();
			}
			ImGui::End();
		}
	}
#pragma endregion

#pragma region Dialog

void MainWindow::OpenDialog(const std::string& key, const std::string& title, const char* filters, const std::string& filePathName, std::function<void(const std::string&, const std::string&)> function, int countSelectionMax, IGFD::UserDatas userData, ImGuiFileDialogFlags flags)
{
	ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, filePathName, countSelectionMax, userData, flags);

	m_DialogToUpdate[key] = function;
}

void MainWindow::UpdateDialogs()
{
	for (auto& dialog : m_DialogToUpdate)
	{
		if (ImGuiFileDialog::Instance()->Display(dialog.first, ImGuiWindowFlags_NoDocking, ImVec2(350, 300))) 
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				if (!fileName.empty())
				{
					// action
					Util::SanitizeFilePath(filePathName);
					Util::SanitizeFilePath(filePath);
					dialog.second(filePathName, filePath);
				}			
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}
	}
}

void MainWindow::SetWorkSpaceDir(const std::string& dir)
{
	this->m_WorkSpaceDir = dir;
	if (m_WorkSpaceDir.back() != '/')
		m_WorkSpaceDir.push_back('/');
}
#pragma endregion
}