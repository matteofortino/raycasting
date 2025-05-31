#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

struct Vec2 {
    float x, y;
};

float distance(const Vec2& a, const Vec2& b) {
    return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

float raySphereIntersection(const Vec2& origin, const Vec2& dir, const Vec2& center, float radius) {
    Vec2 oc = { origin.x - center.x, origin.y - center.y };
    float a = dir.x * dir.x + dir.y * dir.y;
    float b = 2.0f * (oc.x * dir.x + oc.y * dir.y);
    float c = oc.x * oc.x + oc.y * oc.y - radius * radius;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return -1.0f;
    float sqrtDisc = std::sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2 * a);
    float t2 = (-b + sqrtDisc) / (2 * a);
    if (t1 > 0) return t1;
    if (t2 > 0) return t2;
    return -1.0f;
}

void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; 
            int dy = radius - h; 
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("2D Raycasting Sphere",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const Vec2 center = { WIDTH / 2.0f, HEIGHT / 2.0f };

    Vec2 sourcePos = { 0, 0 };
    const float sourceRadius = 15.0f;

    Vec2 blockerPos = center;
    const float blockerRadius = 80.0f; // più grande

    bool running = true;
    SDL_Event e;

    float angle = 0.0f;
    const float angularSpeed = 0.01f; // rad/s

    enum Mode { AUTOMATIC, DRAG };
    Mode mode = AUTOMATIC;

    bool mouseDown = false;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_a) mode = AUTOMATIC;
                else if (e.key.keysym.sym == SDLK_d) mode = DRAG;
            }

            if (mode == DRAG) {
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    mouseDown = true;
                }
                if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                    mouseDown = false;
                }
                if (e.type == SDL_MOUSEMOTION && mouseDown) {
                    sourcePos.x = static_cast<float>(e.motion.x);
                    sourcePos.y = static_cast<float>(e.motion.y);
                }
            }
        }

        if (mode == AUTOMATIC) {
            angle += angularSpeed;
            if (angle > 2.0f * M_PI) angle -= 2.0f * M_PI;
            sourcePos.x = center.x + 150.0f * std::cos(angle);
            sourcePos.y = center.y + 150.0f * std::sin(angle);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Disegna sorgente (sole) come cerchio giallo
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        drawFilledCircle(renderer, static_cast<int>(sourcePos.x), static_cast<int>(sourcePos.y), static_cast<int>(sourceRadius));

        // Disegna ostacolo (terra) come cerchio blu
        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
        drawFilledCircle(renderer, static_cast<int>(blockerPos.x), static_cast<int>(blockerPos.y), static_cast<int>(blockerRadius));

        // Emetti raggi a 360°
        const int numRays = 360;
        for (int i = 0; i < numRays; ++i) {
            float rayAngle = i * (2.0f * M_PI / numRays);
            Vec2 dir = { std::cos(rayAngle), std::sin(rayAngle) };

            float maxDist = 1000.0f;

            float hitDist = raySphereIntersection(sourcePos, dir, blockerPos, blockerRadius);

            float dist = (hitDist > 0 && hitDist < maxDist) ? hitDist : maxDist;

            Vec2 endPoint = { sourcePos.x + dir.x * dist, sourcePos.y + dir.y * dist };

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer,
                               static_cast<int>(sourcePos.x), static_cast<int>(sourcePos.y),
                               static_cast<int>(endPoint.x), static_cast<int>(endPoint.y));
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
