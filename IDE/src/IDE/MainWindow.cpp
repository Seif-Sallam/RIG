#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "IDE/MainWindow.h"

#include <imguial_term.h>
#include <imguial_button.h>
#include <imgui_internal.h>

#include <fstream>

#include <stdio.h>

namespace IDE
{

	int32_t MainWindow::m_Width = 800;
	int32_t MainWindow::m_Height = 600;
	static void ErrorCallbackFunc(int error, const char *description);
	static void KeyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void ViewPortResizeCallback(GLFWwindow *window, int width, int height);

	MainWindow::MainWindow(uint32_t argc, const char *argv[])
		: m_TextEditorWindowName("Text Editors"), m_LogWindowName("Log"), m_RegisterFileWindowName("Register File")
	{
		InitilizeWindow();
		auto lang = TextEditor::LanguageDefinition::CPlusPlus();

		m_TextEditor.SetLanguageDefinition(lang);
		std::string str = "TEXT\n";
		std::ifstream inputFile("/home/seif-sallam/Desktop/Assembler.txt");
		if (inputFile.fail())
		{
			std::cerr << "File was not opened\n";
		}
		else
		{
			while (!inputFile.eof())
			{
				std::string line;
				std::getline(inputFile, line);
				str += line + '\n';
			}
			inputFile.close();
		}
		m_TextEditor.SetText(str);
	}

	void MainWindow::SetupLayout(ImGuiID &dockSpaceID)
	{
		m_LayoutInitialized = true;
		m_DockID = dockSpaceID;

		ImGui::DockBuilderRemoveNode(m_DockID);							   // Clear out existing layout
		ImGui::DockBuilderAddNode(m_DockID, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(m_DockID, ImVec2((float)m_Width, (float)m_Height));
		m_DockIDLeft = ImGui::DockBuilderSplitNode(m_DockID, ImGuiDir_Left, 0.20f, NULL, &m_DockID);
		m_DockIDRight = ImGui::DockBuilderSplitNode(m_DockID, ImGuiDir_Right, 0.20f, NULL, &m_DockID);
		m_DockIDBottom = ImGui::DockBuilderSplitNode(m_DockID, ImGuiDir_Down, 0.20f, NULL, &m_DockID);
		ImGui::DockBuilderDockWindow(m_TextEditorWindowName.c_str(), m_DockID);
		ImGui::DockBuilderDockWindow(m_LogWindowName.c_str(), m_DockIDBottom);
		ImGui::DockBuilderDockWindow(m_RegisterFileWindowName.c_str(), m_DockIDRight);
		ImGui::DockBuilderFinish(m_DockID);
	}

	void MainWindow::SetupDockspace()
	{
		static const char *dockSpaceTitle = "Dockspace";
		static auto dockSpaceID = ImGui::GetID(dockSpaceTitle);
		if (!m_LayoutInitialized)
			SetupLayout(dockSpaceID);

		{
			ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
													ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::SetNextWindowSize(ImVec2((float)m_Width, (float)m_Height));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::Begin(dockSpaceTitle, (bool *)nullptr, dockSpaceWindowFlags);
			auto menuSize = DockspaceMenuBar();
			ImVec2 windowSize((float)m_Width, (float)m_Height);
			auto size = ImVec2();
			size.y = windowSize.y - menuSize.y - 2.0f;
			ImGui::DockSpace(dockSpaceID, size, ImGuiDockNodeFlags_NoWindowMenuButton);
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

			Update();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			SetupDockspace();
			ImGuiLayer();

			Render();
		}
	}

	void MainWindow::HandleEvents()
	{
		glfwPollEvents();
	}

	void MainWindow::ImGuiLayer()
	{
		RegisterFileImGui();
		LogImGui();
		TextEditorImGui();
	}

	void MainWindow::LogImGui()
	{
		ImGui::Begin(m_LogWindowName.c_str());
		{
			ImGui::TextUnformatted("Log Window Text");
		}
		ImGui::End();
	}

	void MainWindow::RegisterFileImGui()
	{
		ImGui::Begin(m_RegisterFileWindowName.c_str());
		{
			ImGui::TextUnformatted("Register File Window Text");
		}
		ImGui::End();
	}

	void MainWindow::TextEditorImGui()
	{
		auto cpos = m_TextEditor.GetCursorPosition();

		ImGui::Begin(m_TextEditorWindowName.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		{
			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, m_TextEditor.GetTotalLines(),
						m_TextEditor.IsOverwrite() ? "Ovr" : "Ins",
						m_TextEditor.CanUndo() ? "*" : " ",
						m_TextEditor.GetLanguageDefinition().mName.c_str(), "~/Desktop/Assembler.txt");

			m_TextEditor.Render("TextEditor");
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
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
	void ErrorCallbackFunc(int error, const char *description)
	{
		fputs(description, stderr);
	}
}