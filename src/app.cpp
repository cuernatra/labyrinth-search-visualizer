#include "app.h"
#include "mazeStorage.h"

#include <algorithm>
#include <cstdio>
#include <cmath>
#include <random>
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
    refreshNodeWeights(true);
    ImGui::SFML::Init(window);
}

void App::refreshNodeWeights(bool randomizePhases)
{
    if (randomizePhases)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> phaseDist(0.f, 6.2831853f);
        weightPhaseA = phaseDist(rng);
        weightPhaseB = phaseDist(rng);
        weightPhaseC = phaseDist(rng);
    }

    const int clampedMaxWeight = std::clamp(maxNodeWeight, 2, 20);

    for (auto& row : grid.getAllNodes())
    {
        for (auto& node : row)
        {
            if (node.state == NodeState::Wall)
            {
                node.weight = 1;
                continue;
            }

            if (!weightedMode)
            {
                node.weight = 1;
                continue;
            }

            const float x = static_cast<float>(node.pos.col);
            const float y = static_cast<float>(node.pos.row);

            const float waveX = std::sin(x * weightFrequency + weightPhaseA);
            const float waveY = std::sin(y * weightFrequency + weightPhaseB);
            const float waveD = std::sin((x + y) * weightFrequency * 0.7f + weightPhaseC);

            const float combined = (waveX + waveY + waveD) / 3.f;
            const float normalized = std::clamp((combined + 1.f) * 0.5f, 0.f, 1.f);
            const float shaped = std::pow(normalized, std::max(0.1f, weightContrast));

            const int weight = 1 + static_cast<int>(std::round(shaped * static_cast<float>(clampedMaxWeight - 1)));
            node.weight = std::clamp(weight, 1, clampedMaxWeight);
        }
    }
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
    generationStepAccumulator = 0.f;
    bfs.reset(grid);
    dfs.reset(grid);
    dijkstra.reset(grid);
    algorithmRunning = false;
    algorithmElapsedSeconds = 0.f;
    activeAlgorithmIndex = -1;
    algorithmStatus = "Algorithm idle.";
    algorithmResultMetric = "-";

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
            refreshNodeWeights(false);
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
    refreshNodeWeights(true);
}

void App::updateLayout()
{
    const int sizeFromWidth  = (maxGridPixels - marginX * 2) / grid.getWidth();
    const int sizeFromHeight = (maxGridPixels - marginY * 2) / grid.getHeight();

    cellSize = std::min(sizeFromWidth, sizeFromHeight);

    if (cellSize < 1)
        cellSize = 1;

    maxMazeCellSize = cellSize;

    const int windowWidth  = grid.getWidth()  * cellSize + marginX * 2;
    const int windowHeight = grid.getHeight() * cellSize + marginY * 2;

    minWindowWidth = static_cast<unsigned int>(windowWidth);
    minWindowHeight = static_cast<unsigned int>(windowHeight);

    unsigned int desiredWindowWidth = minWindowWidth;
    unsigned int desiredWindowHeight = minWindowHeight;
    sf::Vector2i desiredWindowPosition(500, 25);

    if (window.isOpen())
    {
        const sf::Vector2u currentSize = window.getSize();
        desiredWindowWidth = std::max(desiredWindowWidth, currentSize.x);
        desiredWindowHeight = std::max(desiredWindowHeight, currentSize.y);
        desiredWindowPosition = window.getPosition();
    }

    const sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const bool useFullscreen =
        desiredWindowWidth > desktopMode.width ||
        desiredWindowHeight > desktopMode.height;

    if (useFullscreen)
    {
        desiredWindowWidth = desktopMode.width;
        desiredWindowHeight = desktopMode.height;
    }

    window.create(
        sf::VideoMode(desiredWindowWidth, desiredWindowHeight),
        "Labyrinth Search Visualizer",
        useFullscreen
            ? sf::Style::Fullscreen
            : (sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close)
    );

    window.setView(sf::View(sf::FloatRect(
        0.f,
        0.f,
        static_cast<float>(desiredWindowWidth),
        static_cast<float>(desiredWindowHeight))));

    if (!useFullscreen)
    {
        const int maxX = std::max(0, static_cast<int>(desktopMode.width) - static_cast<int>(desiredWindowWidth));
        const int maxY = std::max(0, static_cast<int>(desktopMode.height) - static_cast<int>(desiredWindowHeight));

        desiredWindowPosition.x = std::clamp(desiredWindowPosition.x, 0, maxX);
        desiredWindowPosition.y = std::clamp(desiredWindowPosition.y, 0, maxY);

        window.setPosition(desiredWindowPosition);
    }
}

void App::run()
{
    sf::Clock deltaClock;

    while (window.isOpen())
    {
        const float deltaSeconds = deltaClock.restart().asSeconds();
        processEvents();

        const sf::Vector2u currentSize = window.getSize();
        window.setView(sf::View(sf::FloatRect(
            0.f,
            0.f,
            static_cast<float>(currentSize.x),
            static_cast<float>(currentSize.y))));

        ImGui::SFML::Update(window, sf::seconds(deltaSeconds));
        update(deltaSeconds);
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

        if (event.type == sf::Event::Resized && !adjustingWindowSize)
        {
            unsigned int adjustedWidth = event.size.width;
            unsigned int adjustedHeight = event.size.height;

            bool sizeChanged = false;

            if (adjustedWidth < minWindowWidth)
            {
                adjustedWidth = minWindowWidth;
                sizeChanged = true;
            }

            if (adjustedHeight < minWindowHeight)
            {
                adjustedHeight = minWindowHeight;
                sizeChanged = true;
            }

            if (sizeChanged)
            {
                adjustingWindowSize = true;
                window.setSize(sf::Vector2u(adjustedWidth, adjustedHeight));
                adjustingWindowSize = false;
            }

            window.setView(sf::View(sf::FloatRect(
                0.f,
                0.f,
                static_cast<float>(adjustedWidth),
                static_cast<float>(adjustedHeight))));
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
        }
    }
}

void App::update(float deltaSeconds)
{
    if (!paused && !selectMaze)
    {
        generationStepAccumulator += generationStepsPerSecond * deltaSeconds;
        generationStepAccumulator = std::min(generationStepAccumulator, static_cast<float>(maxGenerationStepsPerUpdate));
        const int steps = std::min(
            static_cast<int>(std::floor(generationStepAccumulator)),
            maxGenerationStepsPerUpdate);

        if (steps < 1)
            return;

        generationStepAccumulator -= static_cast<float>(steps);

        for (int i = 0; i < steps; ++i)
        {
            if (!mazeRouteGenerated)
            {
                mazeRouteGenerated = generator.generateMazeRoute(grid);
                continue;
            }

            if (!remainMazeGenerated)
            {
                remainMazeGenerated = generator.generateRemainMaze(grid);
                continue;
            }

            if (!MazeReady)
            {
                MazeReady = generator.finalizeMaze(grid);

                if (MazeReady)
                {
                    refreshNodeWeights(true);
                    refreshMazeIds();

                    if (!mazeIds.empty())
                        generatedMazeId = mazeIds.back();
                    else
                        generatedMazeId = -1;

                    selectedMazeIndex = -1;
                    selectedMazeId = -1;
                    generatingNewMaze = false;
                    loadedMazeId = -1;
                    break;
                }

                continue;
            }

            break;
        }
    }

    if (algorithmRunning)
    {
        algorithmElapsedSeconds += deltaSeconds;

        if (activeAlgorithmIndex == 0)
        {
            Bfs::StepResult result = bfs.step(grid, algorithmStepsPerUpdate);

            if (result == Bfs::StepResult::Found)
            {
                algorithmRunning = false;

                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "BFS finished: path found in %.3f s.", algorithmElapsedSeconds);
                algorithmStatus = buffer;

                if (weightedMode)
                    algorithmResultMetric = "Path cost sum: " + std::to_string(bfs.getLastPathCost());
                else
                    algorithmResultMetric = "Path length: " + std::to_string(bfs.getLastPathLength());
            }
            else if (result == Bfs::StepResult::NoPath)
            {
                algorithmRunning = false;

                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "BFS finished: no path in %.3f s.", algorithmElapsedSeconds);
                algorithmStatus = buffer;
                algorithmResultMetric = "-";
            }
            else if (result == Bfs::StepResult::InvalidMaze)
            {
                algorithmRunning = false;
                algorithmStatus = "BFS could not continue (invalid maze state).";
                algorithmResultMetric = "-";
            }
        }
        else if (activeAlgorithmIndex == 1)
        {
            Dfs::StepResult result = dfs.step(grid, algorithmStepsPerUpdate);

            if (result == Dfs::StepResult::Found)
            {
                algorithmRunning = false;

                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "DFS finished: path found in %.3f s.", algorithmElapsedSeconds);
                algorithmStatus = buffer;

                if (weightedMode)
                    algorithmResultMetric = "Path cost sum: " + std::to_string(dfs.getLastPathCost());
                else
                    algorithmResultMetric = "Path length: " + std::to_string(dfs.getLastPathLength());
            }
            else if (result == Dfs::StepResult::NoPath)
            {
                algorithmRunning = false;

                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "DFS finished: no path in %.3f s.", algorithmElapsedSeconds);
                algorithmStatus = buffer;
                algorithmResultMetric = "-";
            }
            else if (result == Dfs::StepResult::InvalidMaze)
            {
                algorithmRunning = false;
                algorithmStatus = "DFS could not continue (invalid maze state).";
                algorithmResultMetric = "-";
            }
        }
        else if (activeAlgorithmIndex == 2)
        {
            Dijkstra::StepResult result = dijkstra.step(grid, algorithmStepsPerUpdate);

            if (result == Dijkstra::StepResult::Found)
            {
                algorithmRunning = false;

                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "Dijkstra finished: path found in %.3f s.", algorithmElapsedSeconds);
                algorithmStatus = buffer;

                if (weightedMode)
                    algorithmResultMetric = "Path cost sum: " + std::to_string(dijkstra.getLastPathCost());
                else
                    algorithmResultMetric = "Path length: " + std::to_string(dijkstra.getLastPathLength());
            }
            else if (result == Dijkstra::StepResult::NoPath)
            {
                algorithmRunning = false;

                char buffer[128];
                std::snprintf(buffer, sizeof(buffer), "Dijkstra finished: no path in %.3f s.", algorithmElapsedSeconds);
                algorithmStatus = buffer;
                algorithmResultMetric = "-";
            }
            else if (result == Dijkstra::StepResult::InvalidMaze)
            {
                algorithmRunning = false;
                algorithmStatus = "Dijkstra could not continue (invalid maze state).";
                algorithmResultMetric = "-";
            }
        }
        else
        {
            algorithmRunning = false;
            algorithmStatus = "Algorithm selection became invalid.";
            algorithmResultMetric = "-";
        }
    }
}

void App::render()
{
    const int windowWidth = static_cast<int>(window.getSize().x);
    const int windowHeight = static_cast<int>(window.getSize().y);

    const float padding = 10.f;
    const float basePanelWidth = std::max(150.f, static_cast<float>(marginX) - 2.f * padding);
    const float sidePanelWidth = basePanelWidth;

    const int centerAreaWidth = std::max(
        120,
        windowWidth - static_cast<int>(2.f * sidePanelWidth + 4.f * padding));
    const int centerAreaHeight = std::max(120, windowHeight - marginY * 2);

    const int dynamicCellFromWidth = centerAreaWidth / grid.getWidth();
    const int dynamicCellFromHeight = centerAreaHeight / grid.getHeight();

    const int uncappedCellSize = std::max(1, std::min(dynamicCellFromWidth, dynamicCellFromHeight));
    cellSize = uncappedCellSize;

    const int mazePixelWidth = grid.getWidth() * cellSize;
    const int mazePixelHeight = grid.getHeight() * cellSize;

    const int centerAreaStartX = static_cast<int>(sidePanelWidth + 2.f * padding);
    const int dynamicMarginX = centerAreaStartX + std::max(0, (centerAreaWidth - mazePixelWidth) / 2);
    const int dynamicMarginY = std::max(marginY, (windowHeight - mazePixelHeight) / 2);

    window.clear(sf::Color(90, 90, 90));

    visualizer.draw(window, grid, cellSize, dynamicMarginX, dynamicMarginY, weightedMode);


    const float controlsPanelY = static_cast<float>(marginY) * 0.2f;
    const float controlsPanelHeight = std::max(120.f, static_cast<float>(window.getSize().y) - controlsPanelY - padding);

    float leftPanelX = padding;
    float leftPanelWidth = sidePanelWidth;

    if (leftPanelWidth < 150.f)
        leftPanelWidth = 150.f;

    ImGui::SetNextWindowPos(ImVec2(leftPanelX, controlsPanelY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, controlsPanelHeight), ImGuiCond_Always);

    ImGui::Begin("Controls", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Start generating maze", ImVec2(leftPanelWidth - 20.f, 30.f)))
    {
        if (possibleToStart) 
        {
            generator.setLoopCarveCount(loopCarveCount);
            selectMaze = false;
            paused = false;
    
            mazeRouteGenerated = false;
            remainMazeGenerated = false;
            MazeReady = false;
            generationStepAccumulator = 0.f;
            possibleToStart = false;
            generatingNewMaze = true;
            loadedMazeId = -1;
            generatedMazeId = -1;
            selectedMazeIndex = -1;
            selectedMazeId = -1;
        }
    }

    ImGui::TextUnformatted("\n");
    ImGui::Separator();

    ImGui::TextUnformatted("New maze size\n");

    const float inputWidth = leftPanelWidth - 90.f;
    ImGui::PushItemWidth(inputWidth > 70.f ? inputWidth : 70.f);
    ImGui::InputInt("Width", &newMazeWidth, 2, 2);
    ImGui::InputInt("Height", &newMazeHeight, 2, 2);
    ImGui::PopItemWidth();

    newMazeWidth = std::clamp(newMazeWidth, minMazeDimension, maxMazeDimension);
    newMazeHeight = std::clamp(newMazeHeight, minMazeDimension, maxMazeDimension);

    if (newMazeWidth % 2 == 0)
        newMazeWidth += (newMazeWidth >= maxMazeDimension ? -1 : 1);

    if (newMazeHeight % 2 == 0)
        newMazeHeight += (newMazeHeight >= maxMazeDimension ? -1 : 1);

    ImGui::TextWrapped("Size must be odd, at least 11x11 and at most 101x101.");
    ImGui::TextUnformatted("\nRandom loops");
    ImGui::PushItemWidth(leftPanelWidth - 28.f);
    ImGui::SliderInt("##LoopCarveCount", &loopCarveCount, 0, maxLoopCarveCount, "%d walls");
    ImGui::InputInt("##LoopCarveCountInput", &loopCarveCount, 1, 10);
    ImGui::PopItemWidth();

    loopCarveCount = std::clamp(loopCarveCount, 0, maxLoopCarveCount);
    generator.setLoopCarveCount(loopCarveCount);
    ImGui::TextWrapped("Affects all new generations.");

    if (ImGui::Button("Create new maze", ImVec2(leftPanelWidth - 28.f, 26.f)))
    {
        generator.setLoopCarveCount(loopCarveCount);
        grid.resize(newMazeWidth, newMazeHeight);
        updateLayout();
        resetApp(false);
        paused = false;
        possibleToStart = false;
        generatingNewMaze = true;
        generationStepAccumulator = 0.f;
    }

    ImGui::TextUnformatted("\n");
    ImGui::Separator();
    ImGui::TextUnformatted("Saved mazes");

    std::string statusText = "Status: None";

    if (generatingNewMaze)
        statusText = "Status: Generating";
    else if (loadedMazeId >= 0)
        statusText = "Status: Loaded #" + std::to_string(loadedMazeId);
    else if (generatedMazeId >= 0)
        statusText = "Status: Generated #" + std::to_string(generatedMazeId);

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
    }

    ImGui::TextUnformatted(statusText.c_str());
    ImGui::TextUnformatted("\n");
    ImGui::Separator();
    ImGui::TextUnformatted("Generation speed");
    ImGui::SliderFloat("##GenerationSpeed", &generationStepsPerSecond, 1.f, 600.f, "%.1f steps/s");
    ImGui::PushItemWidth(leftPanelWidth - 28.f);
    ImGui::InputFloat("##GenerationSpeedInput", &generationStepsPerSecond, 0.5f, 5.f, "%.1f");
    ImGui::PopItemWidth();

    if (generationStepsPerSecond < 1.f)
        generationStepsPerSecond = 1.f;

    if (generationStepsPerSecond > 600.f)
        generationStepsPerSecond = 600.f;

    ImGui::TextUnformatted("\n");
    ImGui::Separator();

    ImGui::TextUnformatted("Cell weights");

    if (ImGui::Checkbox("Enable weighted cells", &weightedMode))
    {
        refreshNodeWeights(false);
        algorithmResultMetric = "-";
    }

    bool slidersChanged = false;
    ImGui::PushItemWidth(leftPanelWidth - 28.f);
    ImGui::TextUnformatted("Max weight");
    slidersChanged |= ImGui::SliderInt("##MaxWeight", &maxNodeWeight, 2, 20);

    ImGui::TextUnformatted("Weight frequency");
    slidersChanged |= ImGui::SliderFloat("##WeightFrequency", &weightFrequency, 0.05f, 0.8f, "%.2f");

    ImGui::TextUnformatted("Weight contrast");
    slidersChanged |= ImGui::SliderFloat("##WeightContrast", &weightContrast, 0.3f, 3.0f, "%.2f");
    ImGui::PopItemWidth();

    if (slidersChanged)
    {
        refreshNodeWeights(false);
        algorithmResultMetric = "-";
    }

    if (ImGui::Button("Reshuffle weights", ImVec2(leftPanelWidth - 20.f, 24.f)))
    {
        refreshNodeWeights(true);
        algorithmResultMetric = "-";
    }

    if (!weightedMode)
        ImGui::TextWrapped("Weights disabled: every cell has cost 1.");
    else
        ImGui::TextWrapped("Smooth terrain weights are shown on every walkable node.");

    ImGui::TextUnformatted("\n");
    ImGui::Separator();

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

    float rightPanelWidth = sidePanelWidth;
    float rightPanelX = static_cast<float>(window.getSize().x) - rightPanelWidth - padding;

    const float rightMainPanelHeight = std::max(120.f, static_cast<float>(window.getSize().y) - controlsPanelY - padding);

    ImGui::SetNextWindowPos(ImVec2(rightPanelX, controlsPanelY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, rightMainPanelHeight), ImGuiCond_Always);

    ImGui::Begin("Algorithms", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    ImGui::TextUnformatted("Algorithm selection");

    const char* algorithms[] = { "BFS", "DFS", "Dijkstra" };
    ImGui::Combo("##AlgorithmCombo", &selectedAlgorithmIndex, algorithms, IM_ARRAYSIZE(algorithms));

    if (ImGui::Button("Run selected algorithm", ImVec2(rightPanelWidth - 20.f, 30.f)))
    {
        if (!MazeReady)
        {
            algorithmStatus = "Maze is not ready yet. Generate or load one first.";
        }
        else
        {
            bfs.reset(grid);
            dfs.reset(grid);
            dijkstra.reset(grid);
            algorithmElapsedSeconds = 0.f;
            activeAlgorithmIndex = selectedAlgorithmIndex;
            algorithmResultMetric = "Running...";

            if (selectedAlgorithmIndex == 0)
            {
                if (bfs.start(grid))
                {
                    algorithmRunning = true;
                    algorithmStatus = "BFS running...";
                }
                else
                {
                    algorithmRunning = false;
                    activeAlgorithmIndex = -1;
                    algorithmStatus = "BFS could not start (missing Start/Goal).";
                    algorithmResultMetric = "-";
                }
            }
            else
            {
                if (selectedAlgorithmIndex == 1)
                {
                    if (dfs.start(grid))
                    {
                        algorithmRunning = true;
                        algorithmStatus = "DFS running...";
                    }
                    else
                    {
                        algorithmRunning = false;
                        activeAlgorithmIndex = -1;
                        algorithmStatus = "DFS could not start (missing Start/Goal).";
                        algorithmResultMetric = "-";
                    }
                }
                else
                {
                    if (dijkstra.start(grid))
                    {
                        algorithmRunning = true;
                        algorithmStatus = "Dijkstra running...";
                    }
                    else
                    {
                        algorithmRunning = false;
                        activeAlgorithmIndex = -1;
                        algorithmStatus = "Dijkstra could not start (missing Start/Goal).";
                        algorithmResultMetric = "-";
                    }
                }
            }
        }
    }

    ImGui::TextUnformatted("\n");
    ImGui::TextUnformatted("Algorithm steps/update");
    ImGui::PushItemWidth(rightPanelWidth - 28.f);
    ImGui::SliderInt("##AlgorithmStepsSlider", &algorithmStepsPerUpdate, 1, 200, "%d");
    ImGui::InputInt("##AlgorithmStepsInput", &algorithmStepsPerUpdate, 1, 10);
    ImGui::PopItemWidth();

    if (algorithmStepsPerUpdate < 1)
        algorithmStepsPerUpdate = 1;

    if (algorithmStepsPerUpdate > 200)
        algorithmStepsPerUpdate = 200;

    ImGui::TextUnformatted("\n");
    ImGui::Separator();
    ImGui::TextWrapped("%s", algorithmStatus.c_str());
    ImGui::TextUnformatted("\n");

    ImGui::Separator();

    ImGui::TextUnformatted("Algorithm elapsed time");
    ImGui::Text("%.3f s", algorithmElapsedSeconds);

    ImGui::TextUnformatted("\n");
    ImGui::Separator();
    ImGui::TextUnformatted("Result metric");
    ImGui::TextWrapped("%s", algorithmResultMetric.c_str());

    ImGui::End();

    window.setView(sf::View(sf::FloatRect(
        0.f,
        0.f,
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y))));

    ImGui::SFML::Render(window);
    window.display();
}