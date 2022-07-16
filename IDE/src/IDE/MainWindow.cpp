#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "IDE/MainWindow.h"

#include <imguial_term.h>
#include <imguial_button.h>

#include <fstream>

#include <stdio.h>

namespace IDE
{
	static void ErrorCallbackFunc(int error, const char *description)
	{
		fputs(description, stderr);
	}

	static void KeyCallbackFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		// close window when ESC has been pressed
		// if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		// glfwSetWindowShouldClose(window, GL_TRUE);
	}

	static void ViewPortResizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	MainWindow::MainWindow(uint32_t argc, const char *argv[])
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

	void MainWindow::InitilizeWindow()
	{
		if (!glfwInit())
			exit(-1);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		const int width = 1366;
		const int height = 768;

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
		ImGui::Begin("Test Window");
		{
			ImGui::Text("Hello, World! %d", 100);

			if (ImGui::Button("Log"))
			{
			}
		}
		ImGui::End();

		auto cpos = m_TextEditor.GetCursorPosition();
		ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
		ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save"))
					{
						auto textToSave = m_TextEditor.GetText();
						/// save text....
					}
					if (ImGui::MenuItem("Quit", "Alt-F4"))
					{
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
				ImGui::EndMenuBar();
			}

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
}