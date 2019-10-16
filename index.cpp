#include <iostream>
#include <SFML/Graphics.hpp>

#define width 1600
#define height 1200

struct complex_number
{
    long double real;
    long double imaginary;
    complex_number(long double rl, long double img)
    {
        real = rl;
        imaginary = img;
    }
    sf::Color in_mandelbrot(uint iterations)
    {
        auto real_base = real;
        auto image_base = imaginary;
        for (int i = 0; i < iterations; i++)
        {
            auto tmp = real;
            real *= real;
            real -= (imaginary * imaginary);
            imaginary = 2 * imaginary * tmp;
            real += real_base;
            imaginary += image_base;
            if (real >= 2 || imaginary >= 2)
            {
                return sf::Color(i * i % 255, i * i * i % 255, i % 255);
            }
        }
        return sf::Color::Black;
    }
};

long double X_MIN = -2;
long double X_MAX = 0.5;
long double Y_MIN = -1;
long double Y_MAX = 1;

unsigned int iterations = 10;

long double xFraction;
long double yFraction;
long double x;
long double y;

complex_number point(0, 0);

void redraw(sf::VertexArray &points)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            xFraction = static_cast<long double>(i) / width;
            yFraction = static_cast<long double>(j) / height;
            x = X_MIN + xFraction * (X_MAX - X_MIN);
            y = Y_MIN + yFraction * (Y_MAX - Y_MIN);
            point.real = x;
            point.imaginary = y;
            points[i * height + j].position = sf::Vector2f(i, j);
            points[i * height + j].color = point.in_mandelbrot(iterations);
        }
    }
}

bool firstFocus = false;
bool focused = true;

int main()
{
    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot", sf::Style::Close);

    sf::VertexArray points(sf::Points, width * height);

    sf::Clock clock;

    redraw(points);

    bool previouslyPressed = false;
    sf::Vector2i topLeft;
    sf::Vector2i bottomRight;
    sf::RectangleShape zoomArea;
    zoomArea.setOutlineColor(sf::Color::Red);
    zoomArea.setOutlineThickness(3.0);
    zoomArea.setFillColor(sf::Color(0, 0, 255, 125));
    zoomArea.setPosition(sf::Vector2f(10, 10));
    while (window.isOpen())
    {
        window.clear();
        window.draw(points);
        sf::Event event;
        while (window.pollEvent(event))
        {

            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Up)
                {
                    iterations *= 2;
                    std::cout << "Resolution: " << iterations << " Iterations Per Pixel"
                              << "\n";
                    redraw(points);
                }
                break;
            case sf::Event::LostFocus:
                focused = false;
                break;
            case sf::Event::GainedFocus:
                focused = true;
                firstFocus = true;
                break;
            default:
                break;
            }
        }
        if (focused && !firstFocus)
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if (previouslyPressed)
                {
                    int xDif = topLeft.x - sf::Mouse::getPosition(window).x;
                    bottomRight = sf::Vector2i(sf::Mouse::getPosition(window).x, topLeft.y - (long double)xDif / ((long double)1600 / 1200));
                }
                else
                {
                    previouslyPressed = true;
                    topLeft = sf::Mouse::getPosition(window);
                    bottomRight = topLeft;
                    zoomArea.setPosition(sf::Vector2f(topLeft.x, topLeft.y));
                }
                zoomArea.setSize(sf::Vector2f(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y));
                window.draw(zoomArea);
            }
            else
            {
                if (previouslyPressed)
                {
                    auto TMP_X = X_MAX - X_MIN;
                    auto TMP_Y = Y_MAX - Y_MIN;
                    X_MAX = X_MIN + ((long double)bottomRight.x / width) * TMP_X;
                    X_MIN = X_MIN + ((long double)topLeft.x / width) * TMP_X;
                    Y_MIN = Y_MIN + ((long double)topLeft.y / height) * TMP_Y;
                    Y_MAX = Y_MIN + (X_MAX - X_MIN) / 1.25;
                    previouslyPressed = false;
                    std::cout << "Zoom: " << (2.5) / (X_MAX - X_MIN) << "\n";
                    redraw(points);
                }
            }
        }
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            firstFocus = false;
        }
        window.display();
    }
    return 0;
}