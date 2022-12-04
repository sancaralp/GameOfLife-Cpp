#include <iostream>
#include <utility>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <windows.h>
#include <ctime>

using std::vector;
using std::cin;
using std::cout;
using std::string;
using std::stringstream;

class GameOfLife {
    vector<vector<int>> neighbourLifeCounts;

public:
    explicit GameOfLife(int size) {
        this->size = size;
        for (int i = 0; i < size; ++i) {
            vector<int> neighbourLifeCountElement;
            vector<bool> lifeElement;
            for (int j = 0; j < size; ++j) {
                neighbourLifeCountElement.push_back(0);
                lifeElement.push_back(false);
            }
            neighbourLifeCounts.push_back(neighbourLifeCountElement);
            lives.push_back(lifeElement);
        }
    }

    void setArray(vector<vector<bool>> arr) {
        lives = std::move(arr);
        calculateNeighbours();
    }


    vector<vector<bool>> lives;
    int size;

    void nextState() {
        vector<vector<bool>> tempLives = lives;
        calculateNeighbours();

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (lives[i][j]) {
                    if (neighbourLifeCounts[i][j] == 2 || neighbourLifeCounts[i][j] == 3)
                        continue;
                    tempLives[i][j] = false;
                    continue;
                }
                if (neighbourLifeCounts[i][j] == 3) {
                    tempLives[i][j] = true;
                }
                lives = tempLives;
            }
        }
        calculateNeighbours();
    }

private:
    void calculateNeighbours() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                neighbourLifeCounts[i][j] = 0;
            }
        }

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (lives[i][j]) {
                    for (int a = i - 1; a <= i + 1; a++) {
                        for (int b = j - 1; b <= j + 1; b++) {
                            if (a >= 0 && b >= 0 && a < size && b < size && (a != i || b != j))
                                neighbourLifeCounts[a][b]++;
                        }
                    }
                }
            }
        }
    }

};

enum E_rotation {
    STOP, RIGHT, LEFT, UP, DOWN, CHOOSE, RESET, FINISH
};
E_rotation rotation;
int cursorX = 0, cursorY = 0;
string outputCanvas;
vector<vector<bool>> outputBools;
bool atStart = true;

void Input() {

    if (GetAsyncKeyState(VK_LEFT)) {
        if (rotation != RIGHT)
            rotation = LEFT;
    } else if (GetAsyncKeyState(VK_RIGHT)) {
        if (rotation != LEFT)
            rotation = RIGHT;
    } else if (GetAsyncKeyState(VK_UP)) {
        if (rotation != DOWN)
            rotation = UP;
    } else if (GetAsyncKeyState(VK_DOWN)) {
        if (rotation != UP)
            rotation = DOWN;
    } else if (GetAsyncKeyState(0x52)) /**R*/{
        rotation = RESET;
    } else if (GetAsyncKeyState(VK_SPACE)) {
        rotation = CHOOSE;
    } else if (GetAsyncKeyState(0x50)) /**P*/{
        rotation = FINISH;
    }
}

void setCanvas(const vector<vector<bool>> vector) {
    cout << std::flush;
    system("CLS");
    outputCanvas.clear();
    for (int x = 0; x < vector.size() * 2 + 3; x++) {
        outputCanvas.push_back('#');
    }

    outputCanvas.push_back('\n');
    for (int y = 0; y < vector.size(); y++) {
        outputCanvas.push_back('#');
        if (0 == cursorX && y == cursorY && atStart)
            outputCanvas.push_back('[');
        else
            outputCanvas.push_back(' ');
        for (int x = 0; x < vector.size(); x++) {
            outputCanvas.push_back(vector[y][x] ? '@' : ' ');

            if (x == cursorX - 1 && y == cursorY && atStart)
                outputCanvas.push_back('[');
            else if (x == cursorX && y == cursorY && atStart)
                outputCanvas.push_back(']');
            else
                outputCanvas.push_back(' ');
        }
        outputCanvas.push_back('#');
        outputCanvas.push_back('\n');
    }

    for (int x = 0; x < vector.size() * 2 + 3; x++) {
        outputCanvas.push_back('#');
    }

    outputCanvas.push_back('\n');

    cout << outputCanvas;
}

void SetupLoop() {
    bool finish = false;
    rotation = STOP;
    setCanvas(outputBools);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        Input();

        if (rotation != STOP) {
            switch (rotation) {
                case STOP:
                    break;
                case RIGHT:
                    if (cursorX < outputBools[0].size() - 1)
                        cursorX++;
                    break;
                case LEFT:
                    if (cursorX > 0)
                        cursorX--;
                    break;
                case UP:
                    if (cursorY > 0)
                        cursorY--;
                    break;
                case DOWN:
                    if (cursorY < outputBools.size() - 1)
                        cursorY++;
                    break;
                case CHOOSE:
                    outputBools[cursorY][cursorX] = !outputBools[cursorY][cursorX];
                    break;
                case RESET:
                    for (int i = 0; i < outputBools.size(); ++i) {
                        for (int j = 0; j < outputBools.size(); ++j) {
                            outputBools[i][j] = false;
                        }
                    }
                    break;
                case FINISH:
                    finish = true;
                    break;
            }
            rotation = STOP;
            if (finish)
                break;

            setCanvas(outputBools);
        }
    }
}

GameOfLife *game;

void Setup() {
    int x;
    atStart = true;
    cout << "Size= ";
    cin >> x;

    game = new GameOfLife(x);
    vector<bool> row(x, false);
    vector<vector<bool>> tempVector(x, row);
    outputBools = tempVector;

    SetupLoop();

    game->setArray(outputBools);

    atStart = false;
    setCanvas(game->lives);
}

void Loop() {
    while (true) {
        Input();
        if (rotation==FINISH) {
            cout<<std::flush;
            system("CLS");
            return;
        }
        game->nextState();
        setCanvas(game->lives);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    Setup();
    Loop();
    main();
    return 0;
}