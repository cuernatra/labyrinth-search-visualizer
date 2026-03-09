#include "app.h"
#include "mazeStorage.h"

#include <algorithm>

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
    int sizeFromWidth = maxGridPixelWidth / grid.getWidth();
    int sizeFromHeight = maxGridPixelHeight / grid.getHeight();

    cellSize = std::min(sizeFromWidth, sizeFromHeight);

    if (cellSize < 1)
        cellSize = 1;

    int windowWidth = grid.getWidth() * cellSize + margin * 2;
    int windowHeight = grid.getHeight() * cellSize + margin * 2;

    window.create(
        sf::VideoMode(windowWidth, windowHeight),
        "Labyrinth Search Visualizer",
        sf::Style::Titlebar | sf::Style::Close
    );
}

void App::run()
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void App::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) window.close();

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape) window.close();
        }
    }
}

void App::update()
{
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
    visualizer.draw(window, grid, cellSize, margin);
    window.display();
}