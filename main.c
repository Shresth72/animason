#include <dlfcn.h>
#include <stdio.h>

#include "plug.h"
#include "raylib/raylib-5.0_linux_amd64/include/raylib.h"

int main() {
  const char *libplug_path = "libplug.so";
  void *libplug = dlopen(libplug_path, RTLD_NOW);
  if (libplug == NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return 1;
  }

  plug_init = dlsym(libplug, "plug_init");
  if (plug_init != NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return 1;
  }

  plug_pre_reload = dlsym(libplug, "plug_pre_reload");
  if (plug_pre_reload != NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return 1;
  }

  plug_post_reload = dlsym(libplug, "plug_post_reload");
  if (plug_post_reload != NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return 1;
  }

  plug_update = dlsym(libplug, "plug_update");
  if (plug_update != NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return 1;
  }

  InitWindow(800, 600, "Animason");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RED);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
