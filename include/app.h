#pragma once

#include "visualizer.h"
#include "grid.h"
#include "mazeGenerator.h"
#include "algorithms/bfs.h"
#include "algorithms/dfs.h"
#include "algorithms/dijkstra.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class App
{
public:
    App();
    void run();

    int getCellSize() const { return cellSize; }

    void updateLayout();

    void resetApp(bool loadSelectedMaze);

    void refreshMazeIds();

private:
    void processEvents();
    void update(float deltaSeconds);
    void render();
    void refreshNodeWeights(bool randomizePhases);

    Grid grid;
    Visualizer visualizer;
    MazeGenerator generator;
    Bfs bfs;
    Dfs dfs;
    Dijkstra dijkstra;

    static constexpr int defaultGridWidth = 21;
    static constexpr int defaultGridHeight = 31;
    static constexpr int minMazeDimension = 11;
    static constexpr int maxMazeDimension = 101;
    static constexpr int maxGenerationStepsPerUpdate = 250;
    static constexpr int maxLoopCarveCount = 50;

    int cellSize = 15;

    bool possibleToContinue = false;
    bool possibleToStart = true;
    int loopCarveCount = 0;
    float generationStepsPerSecond = 60.f;
    float generationStepAccumulator = 0.f;
    int algorithmStepsPerUpdate = 1;
    int selectedAlgorithmIndex = 0;
    int activeAlgorithmIndex = -1;
    std::string algorithmStatus = "Algorithm idle.";
    std::string algorithmResultMetric = "-";
    bool algorithmRunning = false;
    float algorithmElapsedSeconds = 0.f;

    bool weightedMode = false;
    int maxNodeWeight = 9;
    float weightFrequency = 0.22f;
    float weightContrast = 1.25f;
    float weightPhaseA = 0.f;
    float weightPhaseB = 0.f;
    float weightPhaseC = 0.f;

    int selectedMazeId = -1;
    int selectedMazeIndex = -1;
    int newMazeWidth = defaultGridWidth;
    int newMazeHeight = defaultGridHeight;
    int loadedMazeId = -1;
    int generatedMazeId = -1;
    bool generatingNewMaze = false;
    std::vector<int> mazeIds;
    std::vector<std::string> mazeIdLabels;
    bool selectMaze = false;

    static constexpr int marginX = 200;
    static constexpr int marginY = 20;

    static constexpr int maxGridPixels = 950;

    bool mazeRouteGenerated = false;
    bool remainMazeGenerated = false;
    bool MazeReady = false;

    bool paused = true;

    unsigned int minWindowWidth = 0;
    unsigned int minWindowHeight = 0;
    bool adjustingWindowSize = false;
    int maxMazeCellSize = 1;

    sf::RenderWindow window;
};