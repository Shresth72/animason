#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ffmpeg/ffmpeg.h"
#include "nob.h"
#include "raylib/raylib-5.0_linux_amd64/include/raylib.h"
#include "raylib/raylib-5.0_linux_amd64/include/raymath.h"

#define FONT_SIZE 52
#define CELL_WIDTH 100.0f
#define CELL_HEIGHT 100.0f
#define CELL_PAD (CELL_WIDTH * 0.15f)
#define RENDER_WIDTH 1600
#define RENDER_HEIGHT 900
#define RENDER_FPS 30
#define RENDER_DELTA_TIME (1.0f / RENDER_FPS)

// ANIMATION
typedef struct {
  size_t i;
  float duration;
} Animation;

typedef struct {
  float from;
  float to;
  float duration;
} Keyframe;

float keyframes_duration(Keyframe *kfs, size_t kfs_count) {
  float duration = 0.0f;
  for (size_t i = 0; i < kfs_count; i++) {
    duration += kfs[i].duration;
  }
  return duration;
}

static float animation_value(Animation a, Keyframe *kfs, size_t kfs_count) {
  assert(kfs_count > 0);

  Keyframe *kf = &kfs[a.i % kfs_count];
  float t = a.duration / kf->duration;
  t = (sinf(PI * t - PI * 0.5) + 1) * 0.5;
  return Lerp(kf->from, kf->to, t);
}

static void animation_update(Animation *a, float dt, Keyframe *kfs,
                             size_t kfs_count) {
  assert(kfs_count > 0);

  a->i = a->i % kfs_count;
  a->duration += dt;

  while (a->duration >= kfs[a->i].duration) {
    a->duration -= kfs[a->i].duration;
    a->i += (a->i + 1) % kfs_count;
  }
}

// PLUGIN
typedef struct {
  size_t size;

  Animation a;
  FFMPEG *ffmpeg;
  RenderTexture2D screen;
  float rendering_duration;

  // Resoources
  Font font;
  Texture2D tsoding;
} Plug;

static Plug *p = NULL;

static void load_resources(void) {
  p->font = LoadFontEx("./resources/fonts/Roboto.ttf", FONT_SIZE, NULL, 0);
  p->tsoding = LoadTexture("./resources/images/tsoding.png");
}

static void unload_resources(void) {
  UnloadFont(p->font);
  UnloadTexture(p->tsoding);
}

void plug_init(void) {
  p = malloc(sizeof(*p));
  assert(p != NULL);
  memset(p, 0, sizeof(*p));
  p->size = sizeof(*p);
  p->screen = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);

  load_resources();
  TraceLog(LOG_INFO, "\033[0;32m-------------------------------\033[0m");
  TraceLog(LOG_INFO, "\033[0;32mInitialized plugin\033[0m");
  TraceLog(LOG_INFO, "\033[0;32m-------------------------------\033[0m");
}

void *plug_pre_reload(void) {
  unload_resources();
  return p;
}

void plug_post_reload(void *state) {
  p = state;
  if (p->size < sizeof(*p)) {
    TraceLog(
        LOG_INFO,
        "\033[0;32mMigrating plug state schema %zu bytes -> %zu bytes\033[0m",
        p->size, sizeof(*p));
    p = realloc(p, sizeof(*p));
    p->size = sizeof(*p);
  } else if (p->size == sizeof(*p)) {
    TraceLog(LOG_INFO, "\033[0;33mNo changes in plug state schema\033[0m");
  }

  load_resources();
}

#define offset 7
static Keyframe kfs[] = {
    {.from = w / 2 - CELL_WIDTH / 2 - (offset + 0) * (CELL_WIDTH + CELL_PAD),
     .to = w / 2 - CELL_WIDTH / 2 - (offset + 0) * (CELL_WIDTH + CELL_PAD),
     .duration = 0.5},
    {.from = w / 2 - CELL_WIDTH / 2 - (offset + 0) * (CELL_WIDTH + CELL_PAD),
     .to = w / 2 - CELL_WIDTH / 2 - (offset + 1) * (CELL_WIDTH + CELL_PAD),
     .duration = 0.5},
    {.from = w / 2 - CELL_WIDTH / 2 - (offset + 1) * (CELL_WIDTH + CELL_PAD),
     .to = w / 2 - CELL_WIDTH / 2 - (offset + 1) * (CELL_WIDTH + CELL_PAD),
     .duration = 0.5},
    {.from = w / 2 - CELL_WIDTH / 2 - (offset + 2) * (CELL_WIDTH + CELL_PAD),
     .to = w / 2 - CELL_WIDTH / 2 - (offset + 2) * (CELL_WIDTH + CELL_PAD),
     .duration = 0.5},
    {.from = w / 2 - CELL_WIDTH / 2 - (offset + 2) * (CELL_WIDTH + CELL_PAD),
     .to = w / 2 - CELL_WIDTH / 2 - (offset + 2) * (CELL_WIDTH + CELL_PAD),
     .duration = 0.5},
    {.from = w / 2 - CELL_WIDTH / 2 - (offset + 3) * (CELL_WIDTH + CELL_PAD),
     .to = w / 2 - CELL_WIDTH / 2 - (offset + 3) * (CELL_WIDTH + CELL_PAD),
     .duration = 0.5},
    // {.from = w / 2 - CELL_WIDTH / 2 - (offset + 3) * (CELL_WIDTH +
    // CELL_PAD),
    //  .to = w / 2 - CELL_WIDTH / 2 - (offset + 3) * (CELL_WIDTH + CELL_PAD),
    //  .duration = 0.5},
    // {.from = w / 2 - CELL_WIDTH / 2 - (offset + 0) * (CELL_WIDTH +
    // CELL_PAD),
    //  .to = w / 2 - CELL_WIDTH / 2 - (offset + 3) * (CELL_WIDTH + CELL_PAD),
    //  .duration = 0.5},
};
#define kfs_count NOB_ARRAY_LEN(kfs)

void turing_machine_tape(Animation *a, float dt, float w, float h) {
  animation_update(a, dt, kfs, kfs_count);

  Vector2 cell_size = {CELL_WIDTH, CELL_HEIGHT};
  float t = animation_value(*a, kfs, kfs_count);

#if 0
  Color cell_color = ColorFromHSV(0, 0.0, 0.15);
  Color head_color = ColorFromHSV(0, 0.8, 0.8);
  Color background_color = ColorFromHSV(120, 0.0, 0.95);
#else
  Color cell_color = ColorFromHSV(0, 0.0, 0.85);
  Color head_color = ColorFromHSV(0, 0.8, 0.8);
  Color background_color = ColorFromHSV(120, 0.0, 0.05);
#endif

  ClearBackground(background_color);

  for (size_t i = 0; i < 20; ++i) { // + GetMousePosition().x
    Rectangle rec = {
        .x = i * (CELL_WIDTH + CELL_PAD) + t,
        .y = h / 2 - CELL_HEIGHT / 2,
        .width = CELL_WIDTH,
        .height = CELL_HEIGHT,
    };
    DrawRectangleRec(rec, cell_color);

    const char *text = "hi";
    Vector2 text_size = MeasureTextEx(p->font, text, FONT_SIZE, 0);
    Vector2 position = {.x = rec.x, .y = rec.y};
    position = Vector2Add(position, Vector2Scale(cell_size, 0.5));
    position = Vector2Subtract(position, Vector2Scale(text_size, 0.5));
    DrawTextEx(p->font, text, position, FONT_SIZE, 0, background_color);
  }

  float head_thick = 20.0;
  Rectangle rec = {
      .width = CELL_WIDTH + head_thick * 3,
      .height = CELL_HEIGHT + head_thick * 3,
  };
  rec.x = w / 2 - rec.width / 2;
  rec.y = h / 2 - rec.height / 2;
  DrawRectangleLinesEx(rec, head_thick, head_color);
}

void plug_update(void) {
  BeginDrawing();
  if (p->ffmpeg) {
    if (p->rendering_duration >= keyframes_duration(kfs, kfs_count)) {
      ffmpeg_end_rendering(p->ffmpeg, false);
    } else {
      BeginTextureMode(p->screen);

      turing_machine_tape(&p->a, RENDER_FPS, RENDER_WIDTH, RENDER_HEIGHT);
      p->rendering_duration += RENDER_DELTA_TIME;

      EndTextureMode();

      Image image = LoadImageFromTexture(p->screen.texture);
      if (!ffmpeg_send_frame_flipped(p->ffmpeg, image.data, image.width,
                                     image.height)) {
        ffmpeg_end_rendering(p->ffmpeg, false);
        p->ffmpeg = NULL;
      }
      UnloadImage(image);
    }
  } else {
    if (IsKeyPressed(KEY_R)) {
      p->ffmpeg =
          ffmpeg_start_rendering(RENDER_WIDTH, RENDER_HEIGHT, RENDER_FPS);
      p->rendering_duration = 0.0;
    }

    turing_machine_tape(&p->a, GetFrameTime(), GetScreenWidth(),
                        GetScreenHeight());
  }
  EndDrawing();
}
