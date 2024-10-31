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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mpeg2convert.h>

#define WIDHT 600
#define HEIGHT 300
#define BUFFER_SIZE 4096

#define VID "data/eq.mpg"

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

  FILE *videoFile = fopen(VID, "rb");
  if (!videoFile) {
    TraceLog(LOG_ERROR, "could not read the video file");
    exit(EXIT_FAILURE);
  }

  bool knowDims = false;
  float currentPeriod = 1;
  float period = 1;
  int framenum = 0;

  SetTargetFPS(60);
  Image img = {0};
  Texture texture = {0};

  while (!WindowShouldClose()) {
    int lastFrame = framenum;
    currentPeriod += period;
    if (currentPeriod > 1.0) {
      currentPeriod -= 1.0;
      while (lastFrame == framenum) {
        state = mpeg2_parse(decoder);
        switch (state) {
        case STATE_BUFFER:
          size = fread(buffer, 1, BUFFER_SIZE, videoFile);
          mpeg2_buffer(decoder, buffer, buffer + size);
          if (size == 0) {
            rewind(videoFile);
            framenum = 0;
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
              period = (info->sequence->frame_period / 34.07) / 1000 / 60;
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
    BeginDrawing();
    // DrawTextureEx(texture, (Vector2){0, 0}, 0, 1, WHITE);
    DrawTexturePro(texture, (Rectangle){0, 0, texture.width, texture.height},
                   (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                   (Vector2){0, 0}, 0, WHITE);

    EndDrawing();
  }

  UnloadTexture(texture);
  CloseWindow();
  mpeg2_close(decoder);

  return EXIT_SUCCESS;
}
