#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "plug.h"
#include "raylib/raylib-5.0_linux_amd64/include/raylib.h"

const char *libplug_path = "libplug.so";
void *libplug = NULL;

bool reload_libplug() {
  if (libplug != NULL) {
    dlclose(libplug);
  }

  libplug = dlopen(libplug_path, RTLD_NOW);
  if (libplug == NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return false;
  }

  plug_init = dlsym(libplug, "plug_init");
  if (plug_init == NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return false;
  }

  plug_pre_reload = dlsym(libplug, "plug_pre_reload");
  if (plug_pre_reload == NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return false;
  }

  plug_post_reload = dlsym(libplug, "plug_post_reload");
  if (plug_post_reload == NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return false;
  }

  plug_update = dlsym(libplug, "plug_update");
  if (plug_update == NULL) {
    fprintf(stderr, "ERROR: %s\n", dlerror());
    return false;
  }

  return true;
}

int main() {
  if (!reload_libplug())
    return 1;

  float factor = 100.0f;

  InitWindow(16 * factor, 9 * factor, "Animason");
  SetTargetFPS(60);
  plug_init();

  while (!WindowShouldClose()) {
    // hot reloading
    if (IsKeyPressed(KEY_R)) {
      void *state = plug_pre_reload();
      reload_libplug();
      plug_post_reload(state);
    }

    plug_update();
  }

  CloseWindow();

  return 0;
}
