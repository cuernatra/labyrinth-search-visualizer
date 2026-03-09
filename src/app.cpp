#include "app.h"
#include "mazeStorage.h"

#include <algorithm>
#include <imgui.h>
#include <imgui-SFML.h>

App::App()
    : grid(defaultGridWidth, defaultGridHeight)
{
    if (!selectMaze)
    {
        generator.generateMazeBase(grid);
    }
    else
    {
        bool loaded = MazeStorage::loadMaze(grid, selectedMazeId);

        if (loaded)
        {
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
    ImGui::SFML::Init(window);
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
        }
    }

    ImGui::End();

    float bottomPanelHeight = 70.f;
    float bottomPanelY = static_cast<float>(window.getSize().y) - static_cast<float>(marginY) + 10.f;

    bottomPanelY = static_cast<float>(window.getSize().y) - bottomPanelHeight - 10.f;

    ImGui::SetNextWindowPos(ImVec2(leftPanelX, bottomPanelY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, bottomPanelHeight), ImGuiCond_Always);

    ImGui::Begin("Bottom Controls", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    float buttonWidth = (leftPanelWidth - 40.f) / 3.f;

    if (ImGui::Button("Reset", ImVec2(buttonWidth, 30.f)))
    {
        paused = true;

        mazeRouteGenerated = false;
        remainMazeGenerated = false;
        MazeReady = false;

        possibleToStart = true;
        possibleToContinue = false;
        selectMaze = false;

        generator.startFromTheScratch(grid);
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

    if (ImGui::Button("Continue", ImVec2(buttonWidth, 30.f)))
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