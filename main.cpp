#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <sstream>

using namespace std;

// Game state constants
const int STATE_MAIN_MENU = 0;
const int STATE_PLAYING = 1;
const int STATE_PAUSED = 2;
const int STATE_GAME_OVER = 3;
const int STATE_HIGH_SCORES = 4;

const int WINDOW_HEIGHT = 760;
const int WINDOW_WIDTH = 540;

const int rows = 13;
const int cols = 11;
int gems[rows][cols];
bool isDestroyed[rows][cols] = {false};

sf::Texture bgTexture;
sf::Sprite bgSprite;

sf::Texture gemTextures[7];
const char* gemFiles[7] = {
    "C:/Bejeweled/Gems/Amber.png",
    "C:/Bejeweled/Gems/Amethyst.png",
    "C:/Bejeweled/Gems/Diamond.png",
    "C:/Bejeweled/Gems/Emerald.png",
    "C:/Bejeweled/Gems/Ruby.png",
    "C:/Bejeweled/Gems/Topaz.png",
    "C:/Bejeweled/Gems/Sapphire.png"
};

sf::Font font;
sf::Text titleText, scoreText, nameText, mainMenuText, pauseText, gameOverText, highScoresText;
int gameState = STATE_MAIN_MENU;
int score = 0;
char playerName[100] = "Noor";

// Variables to handle gem selection
bool gemSelected = false;
int selectedGemX = -1;
int selectedGemY = -1;

const int MAX_HIGH_SCORES = 10;
int highScores[MAX_HIGH_SCORES] = {0};
char highScoreNames[MAX_HIGH_SCORES][100] = {""};

void loadTextures() {
    if (!bgTexture.loadFromFile("C:/Bejeweled/Backgrounds/backdrop12.png")) {
        cout << "Failed to load background texture" << endl;
        exit(-1);
    }
    bgSprite.setTexture(bgTexture);

    for (int i = 0; i < 7; ++i) {
        if (!gemTextures[i].loadFromFile(gemFiles[i])) {
            cout << "Failed to load gem texture: " << gemFiles[i] << endl;
            exit(-1);
        }
    }
}

void initializeFontAndText() {
    if (!font.loadFromFile("C:/Fonts/Arsenal-Regular.ttf")) {
        cout << "Failed to load font" << endl;
        exit(-1);
    }

    titleText.setFont(font);
    titleText.setString("Bejeweled");
    titleText.setCharacterSize(50);
    titleText.setFillColor(sf::Color(90, 114, 59));
    float titleTextX = (WINDOW_WIDTH - titleText.getLocalBounds().width) / 2.0f;
    float titleTextY = 15.0f;
    titleText.setPosition(titleTextX, titleTextY);

    scoreText.setFont(font);
    scoreText.setString("Score: " + to_string(score));
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color(90, 114, 59));
    float scoreTextX = 30.0f;
    float scoreTextY = titleTextY + titleText.getLocalBounds().height + 10.0f;
    scoreText.setPosition(scoreTextX, scoreTextY);

    nameText.setFont(font);
    nameText.setString("Player: " + string(playerName));
    nameText.setCharacterSize(30);
    nameText.setFillColor(sf::Color(90, 114, 59));
    float nameTextX = 30.0f;
    float nameTextY = scoreTextY + scoreText.getLocalBounds().height + 10.0f;
    nameText.setPosition(nameTextX, nameTextY);

    mainMenuText.setFont(font);
    mainMenuText.setString("Main Menu\n1. Start Game\n2. View High Scores\n3. Quit");
    mainMenuText.setCharacterSize(30);
    mainMenuText.setFillColor(sf::Color(90, 114, 59));
    mainMenuText.setPosition(WINDOW_WIDTH / 4.0, WINDOW_HEIGHT / 4.0);

    pauseText.setFont(font);
    pauseText.setString("Game Paused\nPress P to Resume");
    pauseText.setCharacterSize(30);
    pauseText.setFillColor(sf::Color(90, 114, 59));
    pauseText.setPosition(WINDOW_WIDTH / 4.0, WINDOW_HEIGHT / 4.0);

    gameOverText.setFont(font);
    gameOverText.setString("Game Over\nPress R to Restart");
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(sf::Color(90, 114, 59));
    gameOverText.setPosition(WINDOW_WIDTH / 4.0, WINDOW_HEIGHT / 4.0);

    highScoresText.setFont(font);
    highScoresText.setCharacterSize(30);
    highScoresText.setFillColor(sf::Color(90, 114, 59));
}

void initializeGameGrid() {
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            do {
                gems[i][j] = rand() % 7;
                // Ensure there are no immediate matches at initialization
            } while ((i >= 2 && gems[i][j] == gems[i - 1][j] && gems[i][j] == gems[i - 2][j]) ||
                     (j >= 2 && gems[i][j] == gems[i][j - 1] && gems[i][j] == gems[i][j - 2]));
            isDestroyed[i][j] = false;
        }
    }
}

bool isMoveValid(int x1, int y1, int x2, int y2) {
    // Check if the move is within bounds and adjacent
    return (abs(x1 - x2) + abs(y1 - y2) == 1);
}

bool hasMatch() {
    for (int i = 0; i < rows; ++i) {
        int chainLength = 1;
        for (int j = 1; j < cols; ++j) {
            if (gems[i][j] == gems[i][j - 1]) {
                chainLength++;
            } else {
                if (chainLength >= 3) return true;
                chainLength = 1;
            }
        }
        if (chainLength >= 3) return true;
    }

    for (int j = 0; j < cols; ++j) {
        int chainLength = 1;
        for (int i = 1; i < rows; ++i) {
            if (gems[i][j] == gems[i - 1][j]) {
                chainLength++;
            } else {
                if (chainLength >= 3) return true;
                chainLength = 1;
            }
        }
        if (chainLength >= 3) return true;
    }
    return false;
}

bool swapGems(int x1, int y1, int x2, int y2) {
    int temp = gems[y1][x1];
    gems[y1][x1] = gems[y2][x2];
    gems[y2][x2] = temp;
    if (hasMatch()) {
        return true;
    } else {
        // Swap back if no match is found
        temp = gems[y1][x1];
        gems[y1][x1] = gems[y2][x2];
        gems[y2][x2] = temp;
        return false;
    }
}

void checkMatches() {
    // Horizontal matches
    for (int i = 0; i < rows; ++i) {
        int chainLength = 1;
        for (int j = 1; j < cols; ++j) {
            if (gems[i][j] == gems[i][j - 1]) {
                chainLength++;
            } else {
                if (chainLength >= 3) {
                    for (int k = 0; k < chainLength; ++k) {
                        isDestroyed[i][j - 1 - k] = true;
                    }
                    if (chainLength == 3) {
                        score += 10;
                    } else if (chainLength == 4) {
                        score += 20;
                    } else if (chainLength >= 5) {
                        score += 50;
                    }
                }
                chainLength = 1;
            }
        }
        if (chainLength >= 3) {
            for (int k = 0; k < chainLength; ++k) {
                isDestroyed[i][cols - 1 - k] = true;
            }
            if (chainLength == 3) {
                score += 10;
            } else if (chainLength == 4) {
                score += 20;
            } else if (chainLength >= 5) {
                score += 50;
            }
        }
    }

    // Vertical matches
    for (int j = 0; j < cols; ++j) {
        int chainLength = 1;
        for (int i = 1; i < rows; ++i) {
            if (gems[i][j] == gems[i - 1][j]) {
                chainLength++;
            } else {
                if (chainLength >= 3) {
                    for (int k = 0; k < chainLength; ++k) {
                        isDestroyed[i - 1 - k][j] = true;
                    }
                    if (chainLength == 3) {
                        score += 10;
                    } else if (chainLength == 4) {
                        score += 20;
                    } else if (chainLength >= 5) {
                        score += 50;
                    }
                }
                chainLength = 1;
            }
        }
        if (chainLength >= 3) {
            for (int k = 0; k < chainLength; ++k) {
                isDestroyed[rows - 1 - k][j] = true;
            }
            if (chainLength == 3) {
                score += 10;
            } else if (chainLength == 4) {
                score += 20;
            } else if (chainLength >= 5) {
                score += 50;
            }
        }
    }
}

void dropGems() {
    for (int j = 0; j < cols; ++j) {
        for (int i = rows - 1; i >= 0; --i) {
            if (isDestroyed[i][j]) {
                // Move gems down
                for (int k = i; k > 0; --k) {
                    gems[k][j] = gems[k - 1][j];
                    isDestroyed[k][j] = isDestroyed[k - 1][j];
                }
                // New gem at the top
                gems[0][j] = rand() % 7;
                isDestroyed[0][j] = false;
            }
        }
    }
}

void updateGameGrid() {
    bool hasMatches;
    do {
        hasMatches = false;
        checkMatches();

        // Check if any gems are marked for destruction
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (isDestroyed[i][j]) {
                    hasMatches = true;
                }
            }
        }

        if (hasMatches) {
            dropGems();
        }
    } while (hasMatches);
}

void drawMainMenu(sf::RenderWindow &window) {
    window.draw(mainMenuText);
}

void drawPlaying(sf::RenderWindow &window) {
    window.draw(titleText);
    scoreText.setString("Score: " + to_string(score));
    window.draw(scoreText);
    nameText.setString("Player: " + string(playerName));
    window.draw(nameText);

    float scaleFactor = 0.5f;
    float spacingFactor = 44.0f;
    float startX = 30.0f;
    float startY = 150.0f;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
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

void drawPaused(sf::RenderWindow &window) {
    window.draw(pauseText);
}

void drawGameOver(sf::RenderWindow &window) {
    window.draw(gameOverText);
}

void drawHighScores(sf::RenderWindow &window) {
    stringstream ss;
    ss << "High Scores:\n";
    for (int i = 0; i < MAX_HIGH_SCORES; ++i) {
        if (highScores[i] > 0) {
            ss << highScoreNames[i] << ": " << highScores[i] << "\n";
        }
    }
    highScoresText.setString(ss.str());
    window.draw(highScoresText);
}

void loadHighScores() {
    ifstream file("C:/Bejeweled/highscores.txt");
    if (!file) {
        cout << "Failed to load high scores" << endl;
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; ++i) {
        file >> highScores[i];
        file.ignore(); // Ignore the space between score and name
        file.getline(highScoreNames[i], 100);
    }
    file.close();
}

void saveHighScores() {
    ofstream file("C:/Bejeweled/highscores.txt");
    if (!file) {
        cout << "Failed to save high scores" << endl;
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; ++i) {
        file << highScores[i] << " " << highScoreNames[i] << endl;
    }
    file.close();
}

void updateHighScores() {
    int minScoreIndex = 0;
    for (int i = 1; i < MAX_HIGH_SCORES; ++i) {
        if (highScores[i] < highScores[minScoreIndex]) {
            minScoreIndex = i;
        }
    }
    if (score > highScores[minScoreIndex]) {
        highScores[minScoreIndex] = score;
        strncpy(highScoreNames[minScoreIndex], playerName, 100);
        saveHighScores();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Bejeweled");

    loadTextures();
    initializeFontAndText();
    initializeGameGrid();
    loadHighScores();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == STATE_MAIN_MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        gameState = STATE_PLAYING;
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        gameState = STATE_HIGH_SCORES;
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        window.close();
                    }
                }
            } else if (gameState == STATE_PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::P) {
                        gameState = STATE_PAUSED;
                    } else if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                } else if (event.type == sf::Event::MouseButtonPressed) {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;

                    int col = (x - 30) / 44;
                    int row = (y - 150) / 44;

                    if (col >= 0 && col < cols && row >= 0 && row < rows) {
                        if (!gemSelected) {
                            gemSelected = true;
                            selectedGemX = col;
                            selectedGemY = row;
                        } else {
                            if (isMoveValid(selectedGemX, selectedGemY, col, row)) {
                                if (swapGems(selectedGemX, selectedGemY, col, row)) {
                                    updateGameGrid();
                                }
                            }
                            gemSelected = false;
                            selectedGemX = -1;
                            selectedGemY = -1;
                        }
                    }
                }
            } else if (gameState == STATE_PAUSED) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::P) {
                        gameState = STATE_PLAYING;
                    }
                }
            } else if (gameState == STATE_GAME_OVER) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        updateHighScores();
                        score = 0;
                        initializeGameGrid();
                        gameState = STATE_PLAYING;
                    }
                }
            } else if (gameState == STATE_HIGH_SCORES) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        gameState = STATE_MAIN_MENU;
                    }
                }
            }
        }

        window.clear();
        window.draw(bgSprite);

        if (gameState == STATE_MAIN_MENU) {
            drawMainMenu(window);
        } else if (gameState == STATE_PLAYING) {
            drawPlaying(window);
        } else if (gameState == STATE_PAUSED) {
            drawPaused(window);
        } else if (gameState == STATE_GAME_OVER) {
            drawGameOver(window);
        } else if (gameState == STATE_HIGH_SCORES) {
            drawHighScores(window);
        }

        window.display();
    }

    return 0;
}
