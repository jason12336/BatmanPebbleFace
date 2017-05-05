#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_text;
static TextLayer *s_date_text;
static GFont s_time_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  time_t temp = time(NULL);
  struct tm*tick_time = localtime(&temp);
  
  //build the string of the time
  static char s_other[80];
  static char s_buffer[80];
  strftime(s_buffer, sizeof(s_buffer), "%H:%M", tick_time);
  strftime(s_other, sizeof(s_buffer), "%B %d", tick_time);
  text_layer_set_text(s_time_text, s_buffer);
  text_layer_set_text(s_date_text, s_other);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  //get information of the size of the pebble;
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //process and draw the image
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BATMAN_LOGO);
  s_background_layer = bitmap_layer_create(GRect(0, 60, bounds.size.w, 150));
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  //create the textLayer with the specific bounds
  s_time_text = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(65, 60), bounds.size.w, 50)
  );
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BATMAN_FONT_25));
  
  //add specifics of the text
  text_layer_set_font(s_time_text, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_text, GTextAlignmentCenter);
  text_layer_set_font(s_time_text, s_time_font);
  
  //create teh textlayer for date with specific bounds
  s_date_text = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(20, 20), bounds.size.w, 50)
  );
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BATMAN_FONT_25));
  
  //add specifics of the text
  text_layer_set_text_alignment(s_date_text, GTextAlignmentCenter);
  text_layer_set_font(s_date_text, s_time_font);
  
  //add text to the window layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_text));
  layer_add_child(window_layer, text_layer_get_layer(s_date_text));
}

static void main_window_unload(Window *window) {
  //destroy text layer
  text_layer_destroy(s_time_text);
  text_layer_destroy(s_date_text);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}

static void init() {
  //create the window
  s_main_window = window_create();
  //set the load and unload handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  //actually be able to see the window
  window_stack_push(s_main_window, true);
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  //destroy window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}