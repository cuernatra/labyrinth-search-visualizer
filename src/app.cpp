#include "app.h"
#include "mazeStorage.h"

#include <algorithm>
#include <string>
#include <imgui.h>
#include <imgui-SFML.h>

App::App()
    : grid(defaultGridWidth, defaultGridHeight)
{
    refreshMazeIds();

    if (!selectMaze)
    {
        generator.generateMazeBase(grid);
    }
    else
    {
        bool loaded = MazeStorage::loadMaze(grid, selectedMazeId);

        if (loaded)
        {
            possibleToStart = false;
            mazeRouteGenerated = true;
            remainMazeGenerated = true;
            MazeReady = true;
        }
        else
        {
            generator.generateMazeBase(grid);
        }
    }

    updateLayout();
    newMazeWidth = grid.getWidth();
    newMazeHeight = grid.getHeight();
    ImGui::SFML::Init(window);
}

void App::refreshMazeIds()
{
    mazeIds = MazeStorage::getAvailableMazeIds();
    mazeIdLabels.clear();

    for (int id : mazeIds)
        mazeIdLabels.push_back("Maze #" + std::to_string(id));

    if (mazeIds.empty())
    {
        selectedMazeIndex = -1;
        selectedMazeId = -1;
        return;
    }

    if (selectedMazeId >= 0)
    {
        auto selectedIt = std::find(mazeIds.begin(), mazeIds.end(), selectedMazeId);

        if (selectedIt != mazeIds.end())
        {
            selectedMazeIndex = static_cast<int>(std::distance(mazeIds.begin(), selectedIt));
            return;
        }
    }

    selectedMazeIndex = -1;
    selectedMazeId = -1;
}

void App::resetApp(bool loadSelectedMaze)
{
    paused = true;
    possibleToContinue = false;

    mazeRouteGenerated = false;
    remainMazeGenerated = false;
    MazeReady = false;

    if (loadSelectedMaze && selectedMazeId >= 0)
    {
        selectMaze = true;
        bool loaded = MazeStorage::loadMaze(grid, selectedMazeId);

        if (loaded)
        {
            mazeRouteGenerated = true;
            remainMazeGenerated = true;
            MazeReady = true;
            possibleToStart = true;
            updateLayout();
            newMazeWidth = grid.getWidth();
            newMazeHeight = grid.getHeight();
            loadedMazeId = selectedMazeId;
            generatedMazeId = -1;
            generatingNewMaze = false;
            return;
        }
    }

    selectMaze = false;
    possibleToStart = true;
    loadedMazeId = -1;
    generatedMazeId = -1;
    generatingNewMaze = false;
    selectedMazeIndex = -1;
    selectedMazeId = -1;
    generator.startFromTheScratch(grid);
}

void App::updateLayout()
{
    int sizeFromWidth  = (maxGridPixels - marginX * 2) / grid.getWidth();
    int sizeFromHeight = (maxGridPixels - marginY * 2) / grid.getHeight();

    cellSize = std::min(sizeFromWidth, sizeFromHeight);

    if (cellSize < 1)
        cellSize = 1;

    int windowWidth  = grid.getWidth()  * cellSize + marginX * 2;
    int windowHeight = grid.getHeight() * cellSize + marginY * 2;

    window.create(
        sf::VideoMode(windowWidth, windowHeight),
        "Labyrinth Search Visualizer",
        sf::Style::Titlebar | sf::Style::Close
    );

    window.setPosition(sf::Vector2i(500, 25));
}

void App::run()
{
    sf::Clock deltaClock;

    while (window.isOpen())
    {
        processEvents();
        ImGui::SFML::Update(window, deltaClock.restart());
        update();
        render();
    }

    ImGui::SFML::Shutdown();
}

void App::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);

        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
        }
    }
}

void App::update()
{
    if (paused)
        return;

    if (!selectMaze)
    {
        if (!mazeRouteGenerated)
        {
            mazeRouteGenerated = generator.generateMazeRoute(grid);
        }
        else if (!remainMazeGenerated)
        {
            remainMazeGenerated = generator.generateRemainMaze(grid);
        }
        else if (!MazeReady)
        {
            MazeReady = generator.finalizeMaze(grid);

            if (MazeReady)
            {
                refreshMazeIds();

                if (!mazeIds.empty())
                    generatedMazeId = mazeIds.back();
                else
                    generatedMazeId = -1;

                selectedMazeIndex = -1;
                selectedMazeId = -1;
                generatingNewMaze = false;
                loadedMazeId = -1;
            }
        }
    }
}

void App::render()
{
    window.clear(sf::Color(90, 90, 90));

    visualizer.draw(window, grid, cellSize, marginX, marginY);

    const float padding = 10.f;


    float leftPanelX = padding;
    float leftPanelWidth = static_cast<float>(marginX) - 2.f * padding;

    if (leftPanelWidth < 150.f)
        leftPanelWidth = 150.f;

    ImGui::SetNextWindowPos(ImVec2(leftPanelX, static_cast<float>(marginY) * 0.2f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, 70.f), ImGuiCond_Always);

    ImGui::Begin("Top Controls", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Start generating maze", ImVec2(leftPanelWidth - 20.f, 30.f)))
    {
        if (possibleToStart) 
        {
            selectMaze = false;
            paused = false;
    
            mazeRouteGenerated = false;
            remainMazeGenerated = false;
            MazeReady = false;
            possibleToStart = false;
            generatingNewMaze = true;
            loadedMazeId = -1;
            generatedMazeId = -1;
            selectedMazeIndex = -1;
            selectedMazeId = -1;
        }
    }

    ImGui::End();

    float bottomPanelHeight = 70.f;
    float bottomPanelY = static_cast<float>(window.getSize().y) - bottomPanelHeight - 10.f;

    const float topPanelY = static_cast<float>(marginY) * 0.2f;
    const float topPanelHeight = 70.f;

    const float middlePanelY = topPanelY + topPanelHeight + 8.f;
    const float availableMiddleHeight = bottomPanelY - middlePanelY - 8.f;
    const float middlePanelHeight = availableMiddleHeight;

    ImGui::SetNextWindowPos(ImVec2(leftPanelX, middlePanelY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, middlePanelHeight), ImGuiCond_Always);

    ImGui::Begin("Maze Selection", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    ImGui::TextUnformatted("Create maze size");

    const float inputWidth = leftPanelWidth - 90.f;
    ImGui::PushItemWidth(inputWidth > 70.f ? inputWidth : 70.f);
    ImGui::InputInt("Width", &newMazeWidth, 2, 2);
    ImGui::InputInt("Height", &newMazeHeight, 2, 2);
    ImGui::PopItemWidth();

    if (newMazeWidth < minMazeDimension)
        newMazeWidth = minMazeDimension;

    if (newMazeHeight < minMazeDimension)
        newMazeHeight = minMazeDimension;

    if (newMazeWidth % 2 == 0)
        newMazeWidth += 1;

    if (newMazeHeight % 2 == 0)
        newMazeHeight += 1;

    ImGui::TextWrapped("Size must be odd and at least 15x15.");

    if (ImGui::Button("Create new maze", ImVec2(leftPanelWidth - 28.f, 26.f)))
    {
        grid.resize(newMazeWidth, newMazeHeight);
        updateLayout();
        resetApp(false);
        paused = false;
        possibleToStart = false;
        generatingNewMaze = true;
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Saved mazes");

    if (mazeIds.empty())
    {
        ImGui::TextWrapped("No saved mazes found.");
    }
    else
    {
        int previousIndex = selectedMazeIndex;
        const char* preview = (selectedMazeIndex >= 0 && selectedMazeIndex < static_cast<int>(mazeIdLabels.size()))
            ? mazeIdLabels[selectedMazeIndex].c_str()
            : "Select maze";

        if (ImGui::BeginCombo("##SavedMaze", preview, ImGuiComboFlags_HeightSmall))
        {
            for (int i = 0; i < static_cast<int>(mazeIds.size()); ++i)
            {
                bool isSelected = (selectedMazeIndex == i);

                if (ImGui::Selectable(mazeIdLabels[i].c_str(), isSelected))
                {
                    selectedMazeIndex = i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (selectedMazeIndex != previousIndex)
        {
            if (selectedMazeIndex >= 0 && selectedMazeIndex < static_cast<int>(mazeIds.size()))
            {
                selectedMazeId = mazeIds[selectedMazeIndex];
                resetApp(true);
            }
        }

        std::string statusText = "Status: None";

        if (generatingNewMaze)
            statusText = "Status: Generating";
        else if (loadedMazeId >= 0)
            statusText = "Status: Loaded #" + std::to_string(loadedMazeId);
        else if (generatedMazeId >= 0)
            statusText = "Status: Generated #" + std::to_string(generatedMazeId);

        ImGui::TextUnformatted(statusText.c_str());
    }

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(leftPanelX, bottomPanelY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, bottomPanelHeight), ImGuiCond_Always);

    ImGui::Begin("Bottom Controls", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    float buttonWidth = (leftPanelWidth - 35.f) / 3.f;

    if (ImGui::Button("Reset", ImVec2(buttonWidth, 30.f)))
    {
        resetApp(false);
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop", ImVec2(buttonWidth, 30.f)))
    {
        if (!paused) {
            paused = true;
            possibleToContinue = true;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Resume", ImVec2(buttonWidth, 30.f)))
    {
        if (possibleToContinue)
        {
            paused = false;
            possibleToContinue = false;
        }
    }

    ImGui::End();

    ImGui::SFML::Render(window);
    window.display();
}