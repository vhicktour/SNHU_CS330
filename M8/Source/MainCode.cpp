#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159f / 180.0f;

void processInput(GLFWwindow* window);

// Enumerations for brick types and states
enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };

// Class for the paddle
class Paddle {
public:
    float x, y, width, height;
    float red, green, blue;

    Paddle(float xx, float yy, float ww, float hh, float rr, float gg, float bb) {
        x = xx;
        y = yy;
        width = ww;
        height = hh;
        red = rr;
        green = gg;
        blue = bb;
    }

    void drawPaddle() {
        glColor3d(red, green, blue);
        glBegin(GL_POLYGON);

        glVertex2d(x + width / 2, y + height / 2);
        glVertex2d(x + width / 2, y - height / 2);
        glVertex2d(x - width / 2, y - height / 2);
        glVertex2d(x - width / 2, y + height / 2);

        glEnd();
    }

    // Move paddle left or right
    void move(float dx) {
        x += dx;
        // Keep paddle within screen bounds
        if (x - width / 2 < -1) x = -1 + width / 2;
        if (x + width / 2 > 1) x = 1 - width / 2;
    }
};

// Class for bricks
class Brick {
public:
    float red, green, blue;
    float x, y, width, height;
    BRICKTYPE brick_type;
    ONOFF onoff;
    int hitPoints; // Number of hits before destruction

    Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh, float rr, float gg, float bb, int hp) {
        brick_type = bt;
        x = xx;
        y = yy;
        width = ww;
        height = hh;
        red = rr;
        green = gg;
        blue = bb;
        onoff = ON;
        hitPoints = hp;
    }

    void drawBrick() {
        if (onoff == ON) {
            glColor3d(red, green, blue);
            glBegin(GL_POLYGON);

            glVertex2d(x + width / 2, y + height / 2);
            glVertex2d(x + width / 2, y - height / 2);
            glVertex2d(x - width / 2, y - height / 2);
            glVertex2d(x - width / 2, y + height / 2);

            glEnd();
        }
    }

    // Update brick color based on hit points
    void updateColor() {
        if (brick_type == DESTRUCTABLE) {
            switch (hitPoints) {
            case 2:
                red = 1.0f; green = 0.5f; blue = 0.0f; // Orange
                break;
            case 1:
                red = 1.0f; green = 0.0f; blue = 0.0f; // Red
                break;
            default:
                break;
            }
        }
    }
};

// Class for circles
class Circle {
public:
    float red, green, blue;
    float radius;
    float x;
    float y;
    float speedX;
    float speedY;
    bool active;

    Circle(float xx, float yy, float rad, float sX, float sY, float r, float g, float b) {
        x = xx;
        y = yy;
        radius = rad;
        speedX = sX;
        speedY = sY;
        red = r;
        green = g;
        blue = b;
        active = true;
    }

    // Check collision with bricks
    void CheckCollision(Brick* brk) {
        if (brk->onoff == ON) {
            // Axis-Aligned Bounding Box (AABB) collision detection
            if (x + radius > brk->x - brk->width / 2 &&
                x - radius < brk->x + brk->width / 2 &&
                y + radius > brk->y - brk->height / 2 &&
                y - radius < brk->y + brk->height / 2) {

                // Reflect circle
                if (brk->brick_type == REFLECTIVE) {
                    // Simple reflection logic
                    speedY = -speedY;
                }
                else if (brk->brick_type == DESTRUCTABLE) {
                    // Decrease hit points and update color
                    brk->hitPoints--;
                    brk->updateColor();
                    if (brk->hitPoints <= 0) {
                        brk->onoff = OFF;
                    }
                    // Reflect circle
                    speedY = -speedY;
                }
            }
        }
    }

    // Check collision with paddle
    void CheckCollision(Paddle* paddle) {
        if (x + radius > paddle->x - paddle->width / 2 &&
            x - radius < paddle->x + paddle->width / 2 &&
            y - radius < paddle->y + paddle->height / 2 &&
            y + radius > paddle->y - paddle->height / 2) {
            // Reflect circle
            speedY = fabs(speedY); // Ensure circle moves upwards
            // Adjust speedX based on where it hits the paddle
            float hitPos = (x - paddle->x) / (paddle->width / 2);
            speedX = hitPos * 0.02f; // Adjust speedX within a range
            // Prevent the ball from getting stuck inside the paddle
            y = paddle->y + paddle->height / 2 + radius;
        }
    }

    // Check collision with screen edges
    void CheckWallCollision() {
        // Left and right walls
        if (x - radius < -1) {
            x = -1 + radius;
            speedX = -speedX;
        }
        if (x + radius > 1) {
            x = 1 - radius;
            speedX = -speedX;
        }
        // Top wall
        if (y + radius > 1) {
            y = 1 - radius;
            speedY = -speedY;
        }
        // Bottom (circle disappears)
        if (y - radius < -1) {
            active = false;
        }
    }

    // Check collision with other circles
    void CheckCollision(std::vector<Circle>& circles) {
        for (auto& other : circles) {
            if (&other != this && other.active) {
                float dx = x - other.x;
                float dy = y - other.y;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < radius + other.radius) {
                    // Merge circles (example action)
                    active = false;
                    other.active = false;
                    // Create a new larger circle
                    Circle newCircle((x + other.x) / 2, (y + other.y) / 2,
                        radius + other.radius, (speedX + other.speedX) / 2,
                        (speedY + other.speedY) / 2, 1.0f, 1.0f, 0.0f);
                    circles.push_back(newCircle);
                    break;
                }
            }
        }
    }

    // Move circle based on speed
    void MoveOneStep() {
        x += speedX;
        y += speedY;
    }

    void DrawCircle() {
        if (active) {
            glColor3f(red, green, blue);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i++) {
                float degInRad = i * DEG2RAD;
                glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
            }
            glEnd();
        }
    }
};

// Global variables
std::vector<Circle> world;
std::vector<Brick> bricks;
Paddle paddle(0.0f, -0.9f, 0.4f, 0.05f, 0.5f, 0.5f, 1.0f);
float paddleSpeed = 0.05f;

int main(void) {
    srand(static_cast<unsigned int>(time(NULL)));

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(640, 640, "VUdeh Bricks 8-2 Assignment", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Create bricks in a grid pattern
    float startX = -0.8f;
    float startY = 0.6f;
    float brickWidth = 0.2f;
    float brickHeight = 0.1f;
    int rows = 3;
    int cols = 8;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float x = startX + j * (brickWidth + 0.02f);
            float y = startY - i * (brickHeight + 0.02f);
            BRICKTYPE type = (i + j) % 2 == 0 ? DESTRUCTABLE : REFLECTIVE;
            int hitPoints = 3;
            float r = type == DESTRUCTABLE ? 0.0f : 0.5f;
            float g = type == DESTRUCTABLE ? 1.0f : 0.5f;
            float b = 0.0f;
            bricks.emplace_back(type, x, y, brickWidth, brickHeight, r, g, b, hitPoints);
        }
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Setup View
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window);

        // Movement and collision detection
        for (auto& circle : world) {
            if (circle.active) {
                circle.MoveOneStep();
                circle.CheckWallCollision();
                circle.CheckCollision(&paddle);

                for (auto& brick : bricks) {
                    circle.CheckCollision(&brick);
                }

                circle.CheckCollision(world);
            }
        }

        // Remove inactive circles
        world.erase(remove_if(world.begin(), world.end(),
            [](Circle& c) { return !c.active; }), world.end());

        // Draw circles
        for (auto& circle : world) {
            circle.DrawCircle();
        }

        // Draw bricks
        for (auto& brick : bricks) {
            brick.drawBrick();
        }

        // Draw paddle
        paddle.drawPaddle();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void processInput(GLFWwindow* window) {
    static bool spacePressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Paddle movement
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        paddle.move(-paddleSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        paddle.move(paddleSpeed);

    // Launch circle on space key press
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressed) {
            // Only add a new circle on key press event, not while key is held down
            spacePressed = true;
            float r = static_cast<float>(rand()) / RAND_MAX;
            float g = static_cast<float>(rand()) / RAND_MAX;
            float b = static_cast<float>(rand()) / RAND_MAX;
            float speedX = ((rand() % 200) - 100) / 5000.0f; // Random speed between -0.02 and 0.02
            float speedY = 0.02f; // Initial upward speed
            Circle newCircle(0.0f, -0.85f, 0.03f, speedX, speedY, r, g, b);
            world.push_back(newCircle);
        }
    }
    else {
        spacePressed = false;
    }
}




//In this assignment, I enhanced the 2D animation by rearranging the bricks into a grid pattern at the top of the screen, creating a more engaging visual layout.I added a controllable paddle at the bottom of the screen, allowing user interaction using the left and right arrow keys.The bricks now vary in color and type—some are reflective, while others are destructible and require multiple hits before disappearing.This was achieved by introducing a hitPoints attribute to the Brick class and updating the brick's color as it takes damage.

//To apply physics laws to the circles, I modified their movement to reflect realistic bouncing behavior.Circles now have speedX and speedY attributes, enabling them to move in any direction.When a circle collides with a wall, brick, or the paddle, it reflects appropriately based on collision physics.Additionally, I altered the circles' state upon collision with each other by making them merge into a larger circle, adding an interesting dynamic to the animation.

//The code was refactored for better logical flow and modularity, ensuring there are no syntax errors.I adhered to coding best practices by using meaningful variable names, consistent formatting, and adding descriptive comments to enhance readability and maintainability of the code.