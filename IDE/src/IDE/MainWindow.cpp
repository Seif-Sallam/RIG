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

#include <filesystem>

namespace IDE
{

	int32_t MainWindow::m_Width = 800;
	int32_t MainWindow::m_Height = 600;
	static void ErrorCallbackFunc(int error, const char *description);
	static void KeyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void ViewPortResizeCallback(GLFWwindow *window, int width, int height);

	MainWindow::MainWindow(uint32_t argc, const char *argv[])
		: m_TextEditorWindowName("Text Editors")
		, m_LogWindowName("Log")
		, m_RegisterFileWindowName("Register File")
		, m_ConfigOpened(false)
		, m_LayoutInitialized(false)
		, m_ActiveFontSize(24)
		, m_ActiveFont("Consolos")
	{
		InitilizeWindow();
		ImFontConfig cnfg;
		cnfg.MergeMode =true;
		static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 }; // Will not be copied by AddFont* so keep in scope.
		// AddFont(RESOURCES_DIR "/Fonts/Consolos.ttf", 24);
		// ImGui::GetIO().Fonts[0].AddFontFromFileTTF(RESOURCES_DIR"/fonts/fa-solid-900.ttf", 20, &cnfg, icons_ranges);

		m_TextEditor.SetFunctionTooltips(true);
		m_TextEditor.SetShowWhitespaces(true);
		auto lang = TextEditor::LanguageDefinition::RISCV();
		m_TextEditor.SetColorizerEnable(true);
		m_TextEditor.SetLanguageDefinition(lang);


		this->AddDefaultFonts();



		m_TextEditor.SetText(".data\n\n.text\n");
	}

	void MainWindow::RemoveTTForOTF(std::string& str) const
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

	void MainWindow::AddDefaultFonts()
	{
		m_Fonts.clear();
		namespace fs = std::filesystem;
		fs::path path(RESOURCES_DIR"/fonts/");
		{
			auto& consolosFont = m_Fonts[m_ActiveFont];
			consolosFont[m_ActiveFontSize] = AddFont(RESOURCES_DIR"/fonts/Consolos.ttf", 24);
			Util::Logger::Warning("{}", m_ActiveFontSize);
		}
		for (auto entry : fs::directory_iterator(path))
		{
			std::string fileName = entry.path().filename().string();
			RemoveTTForOTF(fileName);
			Util::Logger::Info("Adding font: {}", fileName);
			for(uint32_t fontSize = 18; fontSize <= 48; fontSize+=2)
				m_Fonts[fileName][fontSize] = AddFont(entry.path().string().c_str(), fontSize);
		}
	}

	ImFont* MainWindow::AddFont(const char* path, uint32_t pixelSize)
	{
		auto font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path, (float)pixelSize);
		Util::Logger::Info("Loaded font: {}", path);
		return font;
	}

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
					auto textToSave = m_TextEditor.GetText();
					/// save text....
				}
				if (ImGui::MenuItem("Config"))
					m_ConfigOpened = true;
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
	void MainWindow::InitilizeWindow()
	{
		if (!glfwInit())
			exit(-1);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		const int width = m_Width;
		const int height = m_Height;

		m_Window = glfwCreateWindow(width, height, "RIG v0.0.2", NULL, NULL);

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

	MainWindow::~MainWindow()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void MainWindow::Run()
	{
		while (!glfwWindowShouldClose(m_Window))
		{
			HandleEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::PushFont(m_Fonts[m_ActiveFont][m_ActiveFontSize]);

			SetupDockspace();
			ImGuiLayer();

			ImGui::PopFont();
			Render();
			Update();
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

	void MainWindow::ConfigWindow()
	{
		static std::string selectedFont = m_ActiveFont;
		if (m_ConfigOpened)
		{
			ImGuiWindowFlags configsWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
												  | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::Begin("Config", &m_ConfigOpened, configsWindowFlags);
			{
				static int32_t pixelSize = m_ActiveFontSize;
				// Begin a combo box that contains the options for the fonts
				uint32_t currentFontSize = m_ActiveFontSize;
				ImGui::Text("Active Font: %s, Size: %u", m_ActiveFont.c_str(), currentFontSize);
				if (ImGui::BeginCombo("###1", selectedFont.c_str()))
				{
					for (auto& entry : m_Fonts)
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
				bool activeButton = selectedFont != m_ActiveFont || pixelSize != currentFontSize;
				if (ImGuiAl::Button("Change Font", activeButton))
				{
					pixelSize = (pixelSize % 2) ? pixelSize + 1 : pixelSize;
					m_ActiveFont = selectedFont;
					m_ActiveFontSize = pixelSize;
					selectedFont = m_ActiveFont;
					pixelSize = m_ActiveFontSize;
				}
			}
			ImGui::End();
		}
		else
		{
			selectedFont = m_ActiveFont;
		}

	}

	void MainWindow::LogWindow()
	{
		Util::Logger::Draw(m_LogWindowName.c_str(), nullptr, ImGuiWindowFlags_None);
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

		ImGui::Begin(m_TextEditorWindowName.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s",
						cpos.mLine + 1, cpos.mColumn + 1,
						m_TextEditor.GetTotalLines(),
						m_TextEditor.IsOverwrite() ? "Ovr" : "Ins",
						m_TextEditor.CanUndo() ? "*" : " ",
						m_TextEditor.GetLanguageDefinition().mName.c_str(), "~/Desktop/Assembler.txt");
			// ImGui::PushFont(m_Fonts[m_ActiveFontIndex]);
			m_TextEditor.Render("TextEditor");
			// ImGui::PopFont();
		}
		ImGui::End();
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
	void ViewPortResizeCallback(GLFWwindow *window, int width, int height)
	{
		MainWindow::SetWidth(width);
		MainWindow::SetHeight(height);
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