/*
 * Copyright (c) 2024 I. Kaan Yilmaz (kaandesu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mpeg2convert.h>

#define WIDHT 720
#define HEIGHT 480
#define BUFFER_SIZE 4096
#define STREAM_FRAME_RATE 60

#define VID "data/eq.mpg"

void rlSetup(Texture *t, Camera *c, Model *p);

int main(void) {
  InitWindow(WIDHT, HEIGHT, "raylib example - mpeg2 decode");

  uint8_t buffer[BUFFER_SIZE];
  mpeg2dec_t *decoder = mpeg2_init();
  if (!decoder) {
    TraceLog(LOG_ERROR, "could not init the decoder");
    exit(EXIT_FAILURE);
  }
  const mpeg2_info_t *info = mpeg2_info(decoder);
  mpeg2_state_t state;
  size_t size;

  FILE *video_file = fopen(VID, "rb");
  if (!video_file) {
    TraceLog(LOG_ERROR, "could not read the video file");
    exit(EXIT_FAILURE);
  }

  bool knowDims = false;
  int frame_count = 0;

  Image img;
  Texture texture;
  Camera cam = {0};
  Model plane_model = {0};
  rlSetup(&texture, &cam, &plane_model);

  int last_frame;
  while (!WindowShouldClose()) {
    last_frame = frame_count;
    while (last_frame == frame_count) {
      state = mpeg2_parse(decoder);
      switch (state) {
      case STATE_BUFFER:
        size = fread(buffer, 1, BUFFER_SIZE, video_file);
        mpeg2_buffer(decoder, buffer, buffer + size);
        if (size == 0) {
          rewind(video_file);
          frame_count = 0;
        }
        break;
      case STATE_SEQUENCE:
        mpeg2_convert(decoder, mpeg2convert_rgb24, NULL);
        break;
      case STATE_SLICE:
      case STATE_END:
      case STATE_INVALID_END:
        if (info->display_fbuf) {
          if (knowDims == false) {
            knowDims = true;
            img.width = info->sequence->width;
            img.height = info->sequence->height;
            img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
            img.mipmaps = 1;
            img.data = (unsigned char *)malloc(img.width * img.height * 3);
            texture = LoadTextureFromImage(img);
            UnloadImage(img);
          }
          UpdateTexture(texture, info->display_fbuf->buf[0]);
          frame_count++;
        }
        break;
      default:
        break;
      }
    }
    BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode3D(cam);
    DrawGrid(50, 1);
    DrawModel(plane_model, (Vector3){0, 0, -10}, 1, WHITE);
    EndMode3D();

    EndDrawing();
  }

  UnloadTexture(texture);
  CloseWindow();
  mpeg2_close(decoder);

  return EXIT_SUCCESS;
}

void rlSetup(Texture *t, Camera *c, Model *p) {
  SetTargetFPS(STREAM_FRAME_RATE);
  *p = LoadModelFromMesh(GenMeshPlane(15, 10, 1, 1));
  SetMaterialTexture(&p->materials[0], MATERIAL_MAP_ALBEDO, *t);
  p->transform = MatrixMultiply(MatrixRotateX(PI / 2), MatrixIdentity());

  c->position = (Vector3){0, 2.0f, 4.0f};
  c->target = (Vector3){0, 1.5f, 0.0f};
  c->up = (Vector3){0, 1.0f, 0.0f};
  c->fovy = 45.0f;
  c->projection = CAMERA_PERSPECTIVE;
}
