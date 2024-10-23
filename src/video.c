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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mpeg2convert.h>

#define BUFFER_SIZE 4096
#define VID "data/eq.mpg"

int main(void) {
  const int screenWidth = 1280;
  const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, "raylib example - MPEG2 video playing");

  uint8_t buffer[BUFFER_SIZE];
  mpeg2dec_t *decoder;
  const mpeg2_info_t *info;
  mpeg2_state_t state;
  size_t size;
  int framenum = 0;
  int gotDims = 0;

  FILE *mpgfile;

  mpgfile = fopen(VID, "rb");
  if (!mpgfile) {
    TraceLog(LOG_INFO, "Could not open file");
    exit(1);
  }

  decoder = mpeg2_init();
  if (decoder == NULL) {
    TraceLog(LOG_INFO, "Could not allocate a decoder object.");
    exit(1);
  }
  info = mpeg2_info(decoder);

  Image img = {0};
  Texture texture = {0};

  Camera camera = {0};
  camera.position = (Vector3){0.0f, 1.0f, 4.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  Mesh mesh = GenMeshCube(1, 1, 1);
  Model model = LoadModelFromMesh(mesh);

  float period = 1;
  float currentPeriod = 1;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    //-----------------------------------------------------
    // Update texture from mpg file
    //-----------------------------------------------------
    int lastFrame = framenum;
    currentPeriod += period;
    if (currentPeriod > 1.0) {
      currentPeriod -= 1.0;
      while (lastFrame == framenum) {
        state = mpeg2_parse(decoder);
        switch (state) {
        case STATE_BUFFER:
          // process a buffer sized chunk at a time
          size = fread(buffer, 1, BUFFER_SIZE, mpgfile);
          mpeg2_buffer(decoder, buffer, buffer + size);
          if (size == 0) {
            rewind(mpgfile);
            framenum = 0;
          }
          break;
        case STATE_SEQUENCE:
          // convert to R8G8B8
          mpeg2_convert(decoder, mpeg2convert_rgb24, NULL);
          break;
        case STATE_SLICE:
        case STATE_END:
        case STATE_INVALID_END:
          // if we have enough data for a frame "render" it to
          // the texture and continue with the rest of the program
          if (info->display_fbuf) {
            if (gotDims == 0) {
              // only done once, when we know the frame size
              gotDims = 1;
              img.width = info->sequence->width;
              img.height = info->sequence->height;
              img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
              img.mipmaps = 1;
              img.data = (unsigned char *)malloc(info->sequence->width *
                                                 info->sequence->height * 3);

              // use a dummy image to create a blank texture with
              texture = LoadTextureFromImage(img);
              // apply the texture to the 3d model
              model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
              UnloadImage(
                  img); // get rid off dummy image used to create texture
              TraceLog(LOG_INFO, "frame period %i",
                       info->sequence->frame_period);
              // 37.04 a magic number
              period = ((info->sequence->frame_period / 37.04) / 1000.0) / 60.0;
              TraceLog(LOG_INFO, "1/60th period %f", period);
            }
            UpdateTexture(texture, info->display_fbuf->buf[0]);
            framenum++;
          }
          break;
        default:
          break;
        }
      }
    }
    // end of mpg to texture

    BeginDrawing();

    // we need to clear because of the depth buffer
    // if 2d only the full screen texture draw would
    // be all you needed
    ClearBackground(BLACK);

    DrawTexturePro(texture, (Rectangle){0, 0, texture.width, texture.height},
                   (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                   (Vector2){0, 0}, 0, WHITE);

    BeginMode3D(camera);
    DrawModel(model, (Vector3){0, 0, 0}, 1, WHITE);
    EndMode3D();

    EndDrawing();
  }

  UnloadTexture(texture);
  UnloadModel(model);
  mpeg2_close(decoder);

  CloseWindow();

  return 0;
}
