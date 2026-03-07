#include "app.h"

App::App()
    : window(sf::VideoMode(800, 600), "Labyrinth Search Visualizer"),
    grid(20,20)
{
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
    window.clear();

    visualizer.draw(window, grid);

    window.display();
}