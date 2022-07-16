#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <imgui.h>
#include <imgui-SFML.h>
#include <imguial_term.h>
#include <TextEditor.h>

namespace IDE
{
	class MainWindow
	{
	public:
		MainWindow(uint32_t argc, const char *argv[]);
		void Run();

	private:
		void HandleEvents();
		void ImGuiLayer();
		void Update(const sf::Time &deltaTime);
		void Render();

	private:
		sf::RenderWindow m_Window;
		sf::Clock m_Clk;
		ImGuiAl::BufferedLog<1024> logger;
		TextEditor m_TextEditor;
	};
}