#include "IDE/MainWindow.h"

#include <imgui-SFML.h>
#include <imgui.h>

#include <imguial_term.h>
#include <imguial_button.h>

#include <fstream>

namespace IDE
{
	MainWindow::MainWindow(uint32_t argc, const char *argv[])
		: m_Window(sf::RenderWindow(sf::VideoMode(1366, 768), "RIG v0.0.2", sf::Style::Default))
	{
		ImGui::SFML::Init(m_Window);
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

	void MainWindow::Run()
	{
		while (m_Window.isOpen())
		{
			sf::Time deltaTime = m_Clk.restart();
			HandleEvents();

			Update(deltaTime);

			ImGuiLayer();

			Render();
		}
	}

	void MainWindow::HandleEvents()
	{
		sf::Event event;
		while (m_Window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			switch (event.type)
			{
			case sf::Event::Closed:
				m_Window.close();
				break;
			case sf::Event::Resized:
			{
				sf::View view;
				view.setSize(m_Window.getSize().x, m_Window.getSize().y);
				view.setCenter(view.getSize() / 2.0f);
				m_Window.setView(view);
			}
			break;
			default:
				break;
			}
		}
	}

	void MainWindow::ImGuiLayer()
	{
		ImGui::Begin("Test Window");
		{
			ImGui::Text("Hello, World! %d", 100);

			if (ImGui::Button("Log"))
			{
				logger.info("SOME INFO");
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
						m_Window.close();
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

	void MainWindow::Update(const sf::Time &deltaTime)
	{
		ImGui::SFML::Update(m_Window, deltaTime);
	}

	void MainWindow::Render()
	{
		m_Window.clear();

		ImGui::SFML::Render(m_Window);
		// this->logger.draw(ImVec2(500, 200));
		m_Window.display();
	}
}