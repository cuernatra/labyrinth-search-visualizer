#include "app.h"
#include "mazeGenerator.h"

App::App()
    : window(sf::VideoMode(
        gridWidth * cellSize + margin * 2, 
        gridHeight * cellSize + margin * 2), 
        "Labyrinth Search Visualizer",
        sf::Style::Titlebar | sf::Style::Close),
        grid(gridWidth, gridHeight)
{
    MazeGenerator generator;
    generator.generateMaze(grid);
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
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
    }
}

void App::update()
{
}

void App::render()
{
    window.clear(sf::Color(90, 90, 90));

    visualizer.draw(window, grid);

    window.display();
}