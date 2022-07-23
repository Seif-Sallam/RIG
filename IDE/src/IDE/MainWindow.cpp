#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "IDE/MainWindow.h"

#include <imguial_term.h>
#include <imguial_button.h>
#include <imgui_internal.h>

#include <fstream>

#include <stdio.h>

#include <Utils/Logger.h>
#include <Utils/Functions.h>

#include <filesystem>

namespace IDE
{

#pragma region Static Variables

	using Log = Util::Logger;
	static MainWindow* s_Instance = nullptr;

	static void ErrorCallbackFunc(int error, const char *description);
	static void KeyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void ViewPortResizeCallback(GLFWwindow *window, int width, int height);
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
	}

	MainWindow::~MainWindow()
	{
		SaveState();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void MainWindow::InitializeWindow()
	{
		if (!glfwInit())
			exit(-1);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_MAXIMIZED, true);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		auto mode = glfwGetVideoMode(monitor);
		m_Height = mode->height;
		m_Width = mode->width;

		m_Window = glfwCreateWindow(m_Width, m_Height, "RIG v0.0.2", NULL, NULL);

		if (!m_Window)
		{
			fprintf(stderr, "Failed to open GLFW window.\n");
			glfwTerminate();
			exit(-1);
		}

		glfwMakeContextCurrent(m_Window);

		if (!gladLoadGL())
		{
			fprintf(stderr, "Couldn't load glad\n");
			glfwTerminate();
			exit(-1);
		}
		const GLubyte *renderer = glGetString(GL_RENDERER);
		const GLubyte *version = glGetString(GL_VERSION);
		fprintf(stdout, "Renderer: %s\n", renderer);
		fprintf(stdout, "OpenGL version supported %s\n", version);

		glfwSetKeyCallback(m_Window, KeyCallbackFunc);

		glClearColor(0.0, 0.0, 0.0, 1.0);

		glfwSetFramebufferSizeCallback(m_Window, ViewPortResizeCallback);
		glfwSwapInterval(1); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
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
			"Font2",
			[](MainWindow* ide)
			{
				ImGui::Separator();
				Util::BeginGroupPanel("Test Panal");
				ImGui::TextUnformatted("hello, world second thing");

				Util::EndGroupPanel();
			}
		));
	}
#pragma endregion

#pragma region Run_Functions

	void MainWindow::Run()
	{
		while (!glfwWindowShouldClose(m_Window))
		{
			HandleEvents();

			Update();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

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
		glfwPollEvents();
	}

	void MainWindow::ImGuiLayer()
	{
		ConfigWindow();
		RegisterFileWindow();
		LogWindow();
		TextEditorWindow();
	}

	void MainWindow::Update()
	{

	}

	void MainWindow::Render()
	{
		glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_Window);
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
					glfwSetWindowShouldClose(m_Window, 1);
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

	void MainWindow::ConfigWindow()
	{
		if(m_ConfigOpened)
	{
			ImGuiWindowFlags configsWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
												  | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("Config", &m_ConfigOpened, configsWindowFlags);
			{
				for(auto& s : m_Settings)
					s.second(this);
				// m_Settings.Execute("Font", this);
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

		static TextEditor::Palette p= {
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
		static std::vector<std::string> titles= {
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

		for(int i = 0; i < (int)32; i++)
		{
			std::string title = titles[i] + "###" +  std::to_string(i);
			ImGui::ColorEdit3(title.c_str(), &cols[i].Value.x);

			uint32_t color = cols[i];
			p[i] = color;
		}
		m_TextEditor.SetPalette(p);

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

	void ViewPortResizeCallback(GLFWwindow *window, int width, int height)
	{
		s_Instance->SetWidth(width);
		s_Instance->SetHeight(height);
		glViewport(0, 0, width, height);
	}
	void KeyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		// close window when ESC has been pressed
		// if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		// 	glfwSetWindowShouldClose(window, GL_TRUE);
	}
	void ErrorCallbackFunc(int error, const char *description)
	{
		fputs(description, stderr);
	}
}