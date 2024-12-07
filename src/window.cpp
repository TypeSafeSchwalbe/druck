
#include <druck/window.hpp>
#include <druck/logging.hpp>
#include <cstdlib>
#include <utility>

namespace druck::window {

    using namespace druck::math;


    void init(const char* title, int width, int height, int fps) {
        SetTraceLogLevel(LOG_ERROR);
        SetTraceLogCallback(&druck::logging::raylib);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(width, height, title);
        SetTargetFPS(fps);
    }

    bool should_close() {
        return WindowShouldClose();
    }

    int width() { return GetScreenWidth(); }
    int height() { return GetScreenHeight(); }
    Vec<2> size() { return Vec<2>(width(), height()); }

    double delta_time() { return GetFrameTime(); }

    void display_buffer(rendering::Surface& buffer) {
        Image img;
        img.data = buffer.color;
        img.width = buffer.width;
        img.height = buffer.height;
        img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        img.mipmaps = 1;
        Texture2D texture = LoadTextureFromImage(img);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        EndDrawing();
        UnloadTexture(texture);
    }

    void close() {
        CloseWindow();
    }

}
