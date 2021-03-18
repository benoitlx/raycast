#include "../headers/Game.h"


// ========================================================================================
// Initialization
// ========================================================================================

void Game::initVariables()
{
    this->window = nullptr;
    this->player = nullptr;
}

void Game::initWindow()
{
    this->videoMode.height = 800;
    this->videoMode.width = 800;


    this->window = new sf::RenderWindow(this->videoMode, "Raycasting Game", sf::Style::Titlebar | sf::Style::Close);

    
    for (unsigned int i = 0; i<map.vecMap.size(); i++)
    {
        for (unsigned int j = 0; j<map.vecMap[i].size(); j++)
        {
            if (map.vecMap[j][i])
            {
                sf::RectangleShape box(sf::Vector2f(map.blocSize, map.blocSize));
                box.setPosition(sf::Vector2f(i*map.blocSize, j*map.blocSize));
                mapShape.push_back(box);
            }
        }
    }

    pl.setRadius(2);
    pl.setFillColor(sf::Color(255, 0, 0));
    pl.setOrigin(sf::Vector2f(2, 2));

    pl.setPosition(sf::Vector2f(player->pos[0], player->pos[1]));

    for (unsigned int i=0; i<videoMode.width; i++)
    {
        sf::VertexArray ray(sf::Lines, 2);
        ray[0].color = sf::Color(0, 255, 0);
        ray[1].color = sf::Color(0, 255, 0);

        vecRays.push_back(ray);
    }
}


void Game::initPlayer()
{
    this->player = new Player(3*map.blocSize, 3*map.blocSize, 90, 300, 100, 75, 90);
}

Game::Game(Map& m) : 
    map(m)
{
    this->initVariables();
    this->initPlayer();
    this->initWindow();
}

Game::~Game()
{
    delete this->window;
    delete this->player;
}







// =================================================================================================
// Functions 
// =================================================================================================

// to close and manage window
void Game::pollEvents()
{
    while (this->window->pollEvent(this->event))
    {
        switch (this->event.type)
        {
            case sf::Event::Closed:
                this->window->close();
                break;
            case sf::Event::KeyPressed:
                if (this->event.key.code == sf::Keyboard::Escape)
                    this->window->close();
                break;
        }
    }
}

void Game::controller(sf::Time dt)
{
    float addx = dCos(player->angle)*player->speedMove*dt.asSeconds();
    float addy = dSin(player->angle)*player->speedMove*dt.asSeconds();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
    {
        if(!map.vecMap[player->pos[1]/map.blocSize][(player->pos[0] + addx)/map.blocSize])
            player->pos[0] += addx;
        
        if(!map.vecMap[(player->pos[1] + addy)/map.blocSize][player->pos[0]/map.blocSize])
            player->pos[1] += addy;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        if(!map.vecMap[player->pos[1]/map.blocSize][(player->pos[0] - addx)/map.blocSize])
            player->pos[0] -= addx;
        
        if(!map.vecMap[(player->pos[1] - addy)/map.blocSize][player->pos[0]/map.blocSize])
            player->pos[1] -= addy;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        player->angle -= player->speedAngle*dt.asSeconds();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        player->angle += player->speedAngle*dt.asSeconds();

    if (player->angle > 360) player->angle = 0;
    if (player->angle < 0) player->angle = 360;

    pl.setPosition(sf::Vector2f(player->pos[0], player->pos[1]));
    pl.setRotation(player->angle);
}

// update Player position, enemies position, actualize variable, chek inputs, manage time...
void Game::update()
{
    sf::Time dt = clock.restart();

    this->pollEvents();
    this->controller(dt);
}


//render map
void Game::render2d(unsigned int it)
{
    for (auto& wall : mapShape)
        this->window->draw(wall);

    for (auto& ray : vecRays)
        this->window->draw(ray);

    this->window->draw(pl);
}

// 3d render of the game
void Game::render3d(unsigned int it)
{
    
}

// render and display all the game
void Game::render()
{
    this->window->clear();

    // for ray in range(widthscreen)
    for (unsigned int it = 0; it<videoMode.width; it++)
    {
        this->raycast(it, player->pos[0], player->pos[1]);
        std::cout << "\n" << std::endl;

        // vecRays[it][0] = sf::Vector2f(player->pos[0], player->pos[1]);
        // vecRays[it][1] = sf::Vector2f(interPos[0], interPos[1]);
        vecRays[it][0] = sf::Vertex(sf::Vector2f(player->pos[0], player->pos[1]), sf::Color(0, 255, 0));
        vecRays[it][1] = sf::Vertex(sf::Vector2f(interPos[0], interPos[1]), sf::Color(0, 255, 0));
        

        this->render3d(it);
        this->render2d(it);
    } 

    this->window->display();
}












// retourne les coord de l'intersection du rayon et du mur, la distance et le type du mur (type à changer) 
void Game::raycast(unsigned int it, float posX, float posY)
{
    /* Raycast

        - met à jour la position de l'intersection
        - met à jour le type du mur intersecté
        - met à jour la longueur du rayon
    */
    std::cout << it << "/" << videoMode.width << "\n";
    std::cout << "Position:\n" ;
    std::cout << posX << " " << posY << "\n";
    
    float nAngle = player->angle + player->fov/2 - it*(player->fov/videoMode.width);

    sf::Vector2f dir = {dCos(nAngle), dSin(nAngle)};
    
    int mapX = int(posX/map.blocSize);
    int mapY = int(posY/map.blocSize);

    double dX = std::abs(map.blocSize/dir.x);
    double dY = std::abs(map.blocSize/dir.y);

    double distX;
    double distY;

    int stepX;
    int stepY;

    int hit = 0;
    int side;

    if (dir.x<0){ stepX = -1; distX = (posX - mapX*map.blocSize) / dir.x; }
    else        { stepX = 1; distX = ((mapX + 1)*map.blocSize - posX) / dir.x; }
    
    if (dir.y<0){ stepY = -1; distY = (posY - mapY*map.blocSize) / dir.y; }
    else        { stepY = 1; distY = ((mapY + 1)*map.blocSize - posY) / dir.y; }

    while (hit == 0)
    {
        if (distY < distX)
        {
            distY += dY;
            mapY += stepY;
            side = 1;
        }else{
            distX += dX;
            mapX += stepX;
            side = 0;
        }

        if (map.vecMap[mapY][mapX]) hit = 1;
    }

    double final_dist;
    if (side) final_dist = distY;
    else      final_dist = distX;

    interPos[0] = posX + final_dist*dir.x;
    interPos[1] = posY + final_dist*dir.y;
}

























const bool Game::running() const
{
    return this->window->isOpen();
}

const bool Game::pause() const
{
    return false;
}



