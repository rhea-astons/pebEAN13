#include <pebble.h>

static Window *window;
static TextLayer *tl_digits;
static TextLayer *tl_name;

static char code[13] = "9782940199617";
static int binaryCode[95];

int pattern[10][6] = {
    {0, 0, 0, 0, 0, 0},   // 0
    {0, 0, 1, 0, 1, 1},   // 1
    {0, 0, 1, 1, 0, 1},   // 2
    {0, 0, 1, 1, 1, 0},   // 3
    {0, 1, 0, 0, 1, 1},   // 4
    {0, 1, 1, 0, 0, 1},   // 5
    {0, 1, 1, 1, 0, 0},   // 6
    {0, 1, 0, 1, 0, 1},   // 7
    {0, 1, 0, 1, 1, 0},   // 8
    {0, 1, 1, 0, 1, 0}    // 9
  };

int abBars[2][10][7] = {
  {
    {0, 0, 0, 1, 1, 0, 1},    // 0
    {0, 0, 1, 1, 0, 0, 1},    // 1
    {0, 0, 1, 0, 0, 1, 1},    // 2
    {0, 1, 1, 1, 1, 0, 1},    // 3
    {0, 1, 0, 0, 0, 1, 1},    // 4
    {0, 1, 1, 0, 0, 0, 1},    // 5
    {0, 1, 0, 1, 1, 1, 1},    // 6
    {0, 1, 1, 1, 0, 1, 1},    // 7
    {0, 1, 1, 0, 1, 1, 1},    // 8
    {0, 0, 0, 1, 0, 1, 1}     // 9
  },
  {
    {0, 1, 0, 0, 1, 1, 1},    // 0
    {0, 1, 1, 0, 0, 1, 1},    // 1
    {0, 0, 1, 1, 0, 1, 1},    // 2
    {0, 1, 0, 0, 0, 0, 1},    // 3
    {0, 0, 1, 1, 1, 0, 1},    // 4
    {0, 1, 1, 1, 0, 0, 1},    // 5
    {0, 0, 0, 0, 1, 0, 1},    // 6
    {0, 0, 1, 0, 0, 0, 1},    // 7
    {0, 0, 0, 1, 0, 0, 1},    // 8
    {0, 0, 1, 0, 1, 1, 1}     // 9
  }
};

int cBars[10][7] = {
    {1, 1, 1, 0, 0, 1, 0},    // 0
    {1, 1, 0, 0, 1, 1, 0},    // 1
    {1, 1, 0, 1, 1, 0, 0},    // 2
    {1, 0, 0, 0, 0, 1, 0},    // 3
    {1, 0, 1, 1, 1, 0, 0},    // 4
    {1, 0, 0, 1, 1, 1, 0},    // 5
    {1, 0, 1, 0, 0, 0, 0},    // 6
    {1, 0, 0, 0, 1, 0, 0},    // 7
    {1, 0, 0, 1, 0, 0, 0},    // 8
    {1, 1, 1, 0, 1, 0, 0}     // 9
};

static void ean2Bin() {
  int first = code[0] - '0';
  int digitPos;
  int digit;
  int i;
  binaryCode[0] = 1;
  binaryCode[1] = 0;
  binaryCode[2] = 1;
  int resPos = 3;
  for (digitPos = 1; digitPos < 7; ++digitPos)
  {
    digit = code[digitPos] - '0';
    for (i = 0; i < 7; i++) {
      binaryCode[resPos] = abBars[pattern[first][digitPos-1]][digit][i];
      resPos++;
    }
  }
  binaryCode[resPos] = 0;
  resPos++;
  binaryCode[resPos] = 1;
  resPos++;
  binaryCode[resPos] = 0;
  resPos++;
  binaryCode[resPos] = 1;
  resPos++;
  binaryCode[resPos] = 0;
  resPos++;
  for (digitPos = 7; digitPos < 13; ++digitPos) {
    digit = code[digitPos] - '0';
    for (i = 0; i < 7; i++) {
      binaryCode[resPos] = cBars[digit][i];
      resPos++;
    }
  }
  binaryCode[resPos] = 1;
  resPos++;
  binaryCode[resPos] = 0;
  resPos++;
  binaryCode[resPos] = 1;
}

static void drawBarCode(Layer* layer, GContext* ctx) {
  int padding = 20;
  for (int i = 0; i < 120; i++) {
    for (int j = 0; j < 96; j++) {
      if (binaryCode[j]) {
        graphics_draw_pixel(ctx, GPoint(j+padding, i));
      }
    }
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  layer_set_update_proc(window_layer, drawBarCode);
  tl_digits = text_layer_create((GRect) {{0, bounds.size.w - 18}, {bounds.size.w, bounds.size.h}});
  text_layer_set_text(tl_digits, code);
  text_layer_set_text_alignment(tl_digits, GTextAlignmentCenter);
  text_layer_set_font(tl_digits, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(tl_digits));
}

static void window_unload(Window *window) {
  text_layer_destroy(tl_digits);
}

static void init(void) {
  ean2Bin();
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
