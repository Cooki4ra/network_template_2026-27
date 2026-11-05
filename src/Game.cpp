
#include "Game.h"
#include "Tile.h"
#include <iostream>

Game::Game(sf::RenderWindow& game_window)
    : window(game_window)
{
}

Game::~Game()
{
    
}

bool Game::init()
{
    board = std::make_shared<Board>(window);
    board->init();  

    font = std::make_unique<sf::Font>();
    font->openFromFile("../Data/Fonts/OpenSans-Bold.ttf");
    host_button = std::make_unique<sf::Text>(*font);
    host_button->setFont(*font);
    host_button->setFillColor(sf::Color::White);
    host_button->setString("Host game");
    host_button->setCharacterSize(50);
    host_button->setPosition({900,100});

    join_button = std::make_unique<sf::Text>(*font);
    join_button->setFont(*font);
    join_button->setString("Join game");
    join_button->setCharacterSize(50);
    join_button->setPosition({ 900,200 });
    join_button->setFillColor(sf::Color::White);
    


    return true;
}

void Game::update(float dt)
{

}

void Game::render()
{ 
    if(game_state == mode::MENU)
    {
        window.draw(*host_button);
        window.draw(*join_button);
    }
    if(game_state == mode::GAME)
    {
        board->render();
    }

}

void Game::mouseButtonPressed(const sf::Event::MouseButtonPressed* event)
{
    board->mouseButtonPressed(event);
    if (game_state == mode::MENU)
    {
        if (host_button->getGlobalBounds().contains(sf::Vector2f(event->position)))
        {
            game_state = mode::GAME;
            std::cout << "creating server" << std::endl;
            server = std::make_unique<Server>();
            server->init();
            server->run();
        }
        else if (join_button->getGlobalBounds().contains(sf::Vector2f(event->position)))
        {
            game_state = mode::GAME;
            std::cout << "Connecting to server" << std::endl;
            client = std::make_unique<Client>();
            client->connect();
            client->run();
        }
    }

}

void Game::mouseButtonReleased(const sf::Event::MouseButtonReleased* event)
{
    board->mouseButtonReleased(event);    
}

void Game::keyPressed(const sf::Event::KeyPressed* event)
{

}

void Game::keyReleased(const sf::Event::KeyReleased* event)
{

}

void Game::mouseMoved(const sf::Event::MouseMoved* event)
{
    board->mouseMoved(event);
}




