#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <input/input.h>
#include "helloflippy_icons.h"

typedef struct {
  uint8_t x, y;
} ImagePosition;

static ImagePosition image_position = {.x = 0, .y = 0};
static void app_draw_callback(Canvas* canvas, void* context);
static void app_input_callback(InputEvent* input_event, void* context);

int32_t helloflippy_main(void* p) {
  UNUSED(p);
  FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

  // Configure viewport
  ViewPort* view_port = view_port_alloc();
  view_port_draw_callback_set(view_port, app_draw_callback, view_port);
  view_port_input_callback_set(view_port, app_input_callback, event_queue);

  // Register viewort in GUI
  Gui* gui = furi_record_open(RECORD_GUI);
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  InputEvent event;

  bool running = true;
  while(running) {
    if (furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
      switch (event.key) {
      case InputKeyLeft:
        image_position.x -= 2;
        break;
      case InputKeyRight:
        image_position.x += 2;
        break;
      case InputKeyUp:
        image_position.y -= 2;
        break;
      case InputKeyDown:
        image_position.y += 2;
        break;     
      default:
        running = false;
        break;
      }
    }
    view_port_update(view_port);
  }

  // Cleanning up
  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  view_port_free(view_port);
  furi_message_queue_free(event_queue);

  furi_record_close(RECORD_GUI);

  return 0;
}

static void app_draw_callback(Canvas* canvas, void* context) {
  UNUSED(context);

  canvas_clear(canvas);
  canvas_draw_icon(canvas, image_position.x % 128, image_position.y % 64, &I_airplane);
}

static void app_input_callback(InputEvent* input_event, void* context) {
  furi_assert(context);

  FuriMessageQueue* event_queue = context;
  furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}