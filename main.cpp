#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    int WINDOW_HEIGHT = 760, WINDOW_WIDTH = 540;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Bejeweled");

    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("C:/Bejeweled/Backgrounds/backdrop12.png")) {
        cout << "Failed to load background texture" << endl;
        return -1;
    }
    sf::Sprite bgSprite;
    bgSprite.setTexture(bgTexture);

    sf::Texture gemTextures[7];
    const string gemFiles[7] = {
        "C:/Bejeweled/Gems/Amber.png",
        "C:/Bejeweled/Gems/Amethyst.png",
        "C:/Bejeweled/Gems/Diamond.png",
        "C:/Bejeweled/Gems/Emerald.png",
        "C:/Bejeweled/Gems/Ruby.png",
        "C:/Bejeweled/Gems/Topaz.png",
        "C:/Bejeweled/Gems/Sapphire.png"
    };

    for (int i = 0; i < 7; ++i) {
        if (!gemTextures[i].loadFromFile(gemFiles[i])) {
            cout << "Failed to load gem texture: " << gemFiles[i] << endl;
            return -1;
        }
    }

    const int rows = 13;
    const int cols = 11;
    int gems[rows][cols];
    bool isDestroyed[rows][cols] = {false};

    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            gems[i][j] = rand() % 7;
        }
    }

    float scaleFactor = 0.5f;
    float spacingFactor = 44.0f;

    // Position to start displaying gems
    float startX = 30.0f;
    float startY = 150.0f;


    sf::Font font;
    if (!font.loadFromFile("C:/Fonts/Arsenal-Regular.ttf ")) {
        cout << "Failed to load font" << endl;
        return -1;
    }

    sf::Text text;
    text.setFont(font);
    text.setString("Bejeweled");
    text.setCharacterSize(50);
    text.setFillColor(sf::Color(90, 114, 59));


    float textX = (WINDOW_WIDTH - text.getLocalBounds().width) / 2.0f;
    float textY = 15.0f;

    text.setPosition(textX, textY);

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;
                    cout << "Mouse clicked at: (" << x << ", " << y << ")" << endl;
                }
            }
        }

        window.clear();
        window.draw(bgSprite);
        window.draw(text);


        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (!isDestroyed[i][j]) {
                    sf::Sprite gemSprite;
                    gemSprite.setTexture(gemTextures[gems[i][j]]);
                    gemSprite.setScale(scaleFactor, scaleFactor);


                    float x = startX + j * spacingFactor;
                    float y = startY + i * spacingFactor;
                    gemSprite.setPosition(x, y);

                    window.draw(gemSprite);
                }
            }
        }

        window.display();
    }

    return 0;
}
