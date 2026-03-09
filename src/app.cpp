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
    visualizer.draw(window, grid, cellSize, marginX, marginY);
    window.display();
}