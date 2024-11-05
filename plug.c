#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "nob.h"
#include "raylib/raylib-5.0_linux_amd64/include/raylib.h"
#include "raylib/raylib-5.0_linux_amd64/include/raymath.h"

typedef struct {
  size_t i;
  float duration;
  bool loop;
} Animation;

typedef struct {
  Color background;
  Animation a;
} Plug;

static Plug *p = NULL;

void plug_init(void) {
  p = malloc(sizeof(*p));
  assert(p != NULL);

  memset(p, 0, sizeof(*p));
  p->background = GREEN;
  p->a.loop = true;

  TraceLog(LOG_INFO, "\033[0;32m-------------------------------\033[0m");
  TraceLog(LOG_INFO, "\033[0;32mInitialized plugin\033[0m");
  TraceLog(LOG_INFO, "\033[0;32m-------------------------------\033[0m");
}

void *plug_pre_reload(void) { return p; }

void plug_post_reload(void *state) { p = state; }

typedef struct {
  float from;
  float to;
  float duration;
} Keyframe;

typedef struct {
  Keyframe *items;
  size_t count;
  size_t capacity;
} Keyframes;

float animation_value(Animation a, Keyframe *kfs, size_t kfs_count) {
  assert(kfs_count > 0);

  if (a.i >= kfs_count) {
    return kfs[kfs_count - 1].to;
  }

  Keyframe *kf = &kfs[a.i];
  return Lerp(kf->from, kf->to, a.duration / kf->duration);
}

void animation_update(Animation *a, Keyframe *kfs, size_t kfs_count) {
  assert(kfs_count > 0);

  if (a->i >= kfs_count) {
    if (a->loop) {
      a->i = 0;
      a->duration = 0;
    } else {
      return;
    }
  }

  float dt = GetFrameTime();
  a->duration += dt;

  while (a->i < kfs_count && a->duration >= kfs[a->i].duration) {
    a->duration -= kfs[a->i].duration;
    a->i += 1;
  }
}

void plug_update(void) {
  Keyframe kfs[] = {
      {
          .from = 0.0,
          .to = 0.250,
          .duration = 0.250,
      },
      {
          .from = 0.25,
          .to = 0.25,
          .duration = 0.5,
      },
      {
          .from = 0.25,
          .to = 1.0,
          .duration = 0.5,
      },
  };

  float w = GetScreenWidth();
  float h = GetScreenHeight();

  BeginDrawing();
  animation_update(&p->a, kfs, NOB_ARRAY_LEN(kfs));

  float t = animation_value(p->a, kfs, NOB_ARRAY_LEN(kfs));

  ClearBackground(GetColor(0x181818FF));

  float rw = 100.0;
  float rh = 100.0;
  float pad = rw * 0.15;
  Color cell_color = ColorFromHSV(0, 0.8, 1);

  for (size_t i = 0; i < 20; ++i) { // + GetMousePosition().x
    DrawRectangle(i * (rw + pad) - w * t, h / 2 - rh / 2, rw, rh, cell_color);
  }

  float head_thick = 20.0;
  Rectangle rec = {
      .width = rw + head_thick * 3,
      .height = rh + head_thick * 3,
  };
  rec.x = w / 2 - rec.width / 2;
  rec.y = h / 2 - rec.height / 2;
  DrawRectangleLinesEx(rec, head_thick, YELLOW);

  EndDrawing();
}
