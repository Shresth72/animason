#include "./raylib/raylib-5.0_linux_amd64/include/raylib.h"

int main() {
  InitWindow(800, 600, "Animason");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RED);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
