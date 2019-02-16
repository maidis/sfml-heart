#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <random>

std::tuple<std::vector<double>, std::vector<double>> generate_points(const int no_pieces)
{
    std::vector<double> vx, vy;

    constexpr double PI = 3.14159265358979323846264338327950288;
    double dt = 2.0 * PI / no_pieces;

    for(double t = 0.0; t <= 2.0 * PI ; t += dt)
    {
        vx.emplace_back(16.0 * sin(t) * sin(t) * sin(t));
        vy.emplace_back(13.0 * cos(t) - 5.0 * cos(2.0 * t) - 2.0 * cos(3.0 * t) - cos(4.0 * t));
    }

    return std::make_tuple(vx, vy);
}

// The frequency at which our update step will execute
#define UPDATE_RATE 1.0f / 60.0f

int main()
{
    // Generate the heart curve points
    int no_pieces = 300;
    auto [vx, vy] = generate_points(no_pieces);

    // Reverse Y axis direction
    std::for_each(std::begin(vy), std::end(vy), [](double &y){ y = -y; });

    std::vector<sf::ConvexShape> line;
    sf::Color color(255, 0, 0);

    for (int j = 0; j < 50; ++j)
    {
        std::random_device rd;
        std::mt19937 mt(rd());

        std::uniform_real_distribution<double> dist0(1.0, 1920.0);
        std::uniform_real_distribution<double> dist1(1.0, 1080.0);
        std::uniform_real_distribution<double> dist2(1.0, 10.0);

        std::uniform_real_distribution<double> r(0.0, 5.0);
        std::uniform_real_distribution<double> g(0.0, 2.0);
        std::uniform_real_distribution<double> b(0.0, 2.0);

        sf::ConvexShape lines;
        lines.setPointCount(no_pieces);

        for (int i = 0; i < no_pieces; ++i)
        {
            lines.setPoint(i, sf::Vector2f(vx[i], vy[i]));
        }

        int scale = dist2(mt);

        int rr = r(mt);
        int gg = g(mt);
        int bb = b(mt);

        color.r = color.r - rr;
        color.g = color.g + gg;
        color.b = color.b + bb;

        lines.setFillColor(color);
        lines.setOutlineThickness(0);
        lines.setPosition(dist0(mt), dist1(mt));
        lines.setScale(scale, scale);

        line.push_back(lines);
    }

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "sfml-heart", sf::Style::Fullscreen);

    // This forces our window to draw at refresh rate of the monitor
    // Uncomment the line below to check out the fps counter
    //window.setVerticalSyncEnabled(true);

    sf::RectangleShape player(sf::Vector2f(50.0f, 50.0f));
    player.setOrigin(player.getSize().x / 2, player.getSize().y / 2);
    player.setPosition(window.getSize().x / 2, window.getSize().y /2);
    player.setFillColor(sf::Color::Red);

    // The sf::Clock class will allow us to keep track of time
    sf::Clock clock;
    // This variable accumulates the time spent during render steps
    float total_time = 0.0f;
    // This variable holds the amount of time the last render step took
    float frame_time = 0.0f;
    // This variable keeps track of how many times the update step ran
    int update_limit = 0;

    while(window.isOpen())
    {
        // Get how long it took to draw the last frame
        frame_time = clock.restart().asSeconds();
        // Add that time to the total time
        total_time += frame_time;
        // Reset our counter
        update_limit = 0;

        // While the total amount of time spend on the render step is
        // greater or equal to the update rate (1/60) and we have
        // not executed the update step 10 times then do the loop
        // If the counter hits 10 we break because it means that the
        // render step is lagging behind the update step
        while(total_time >= UPDATE_RATE && update_limit < 10)
        {
            sf::Event evt;

            while(window.pollEvent(evt))
            {
                if(evt.type == sf::Event::Closed)
                    window.close();
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                player.move(0.0f, -5.0f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                player.move(0.0f, 5.0f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                player.move(-5.0f, 0.0f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                player.move(5.0f, 0.0f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            for (int i = 0; i < line.size(); ++i)
            {
                int speed = 1;
                if ((i % 10) == 0)
                {
                    speed = speed + 1;
                }
                if (line.at(i).getPosition().y < 1080)
                {
                    line.at(i).move(0.0f, 1.0f + speed);
                }
                else
                {
                    line.at(i).setPosition(line.at(i).getPosition().x, 0);
                }

                if (player.getGlobalBounds().intersects(line.at(i).getGlobalBounds()))
                {
                    line.erase(line.begin() + i);
                }
            }

            if (line.size() <= 0)
            {
                sf::Text text;
                sf::Font font;
                font.loadFromFile("/usr/share/fonts/google-noto/NotoSans-Black.ttf");
                text.setFont(font);
                text.setString("I love U");
                text.setCharacterSize(240);
                text.setPosition(window.getSize().x / 4, window.getSize().y / 4);
                text.setFillColor(sf::Color::Red);
                window.draw(text);
                window.display();
                sf::sleep(sf::seconds(3.0f));
                window.close();
            }

            player.rotate(1.0f);

            // Subtract the udpate frequency from the total time
            total_time -= UPDATE_RATE;
            // Increase the counter
            update_limit++;
        }

        window.clear();
        window.draw(player);
        for (int i = 0; i < line.size(); ++i)
        {
            window.draw(line.at(i));
        }
        window.display();
    }

    return 0;
}
