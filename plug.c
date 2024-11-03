#include <stdlib.h>

#include "nob.h"
#include "raylib/raylib-5.0_linux_amd64/include/raylib.h"

typedef struct {
  Color background;
} Plug;

static Plug *p = NULL;

void plug_init(void) {
  p = malloc(sizeof(*p));
  assert(p != NULL);
}

void *plug_pre_reload(void) { return p; }

void plug_post_reload(void *state) { p = state; }

void plug_update(void) {
  BeginDrawing();
  ClearBackground(p->background);
  EndDrawing();
}
