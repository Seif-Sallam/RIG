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

	private:
		void HandleEvents();
		void ImGuiLayer();
		void Update();
		void Render();

		void InitilizeWindow();

	private:
		GLFWwindow *m_Window;
		TextEditor m_TextEditor;
	};

}