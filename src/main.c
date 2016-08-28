#include <pebble.h>
static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
//Rotating
static BitmapLayer *s_ring_parent_layer;
static RotBitmapLayer *s_out_ring_layer;
static GBitmap *s_out_ring_bitmap;
static BitmapLayer *s_bg_parent_layer;
static RotBitmapLayer *s_bg_layer;
static GBitmap *s_bg_bitmap;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
	
	double	dy,dx;
	//Set Roll Angle (Hours)
	int32_t angle = TRIG_MAX_ANGLE/720 * ((tick_time->tm_hour%12)*60 + tick_time->tm_min);
	//Set Pitch Angle (Minutes)
	if (tick_time->tm_min <= 30){
		dy = - (tick_time->tm_min)*8.5/5*-cos_lookup(angle)/TRIG_MAX_ANGLE;
		dx = - (tick_time->tm_min)*8.5/5*(sin_lookup(angle))/TRIG_MAX_ANGLE;
	} else {
		dy = - (tick_time->tm_min-60)*8.5/5*-cos_lookup(angle)/TRIG_MAX_ANGLE;
		dx = - (tick_time->tm_min-60)*8.5/5*+sin_lookup(angle)/TRIG_MAX_ANGLE;
	}
	
	//Pitch ando Roll!!
	rot_bitmap_layer_set_angle(s_out_ring_layer,angle);
	rot_bitmap_layer_set_angle(s_bg_layer,angle);
	layer_set_frame((Layer *)s_bg_parent_layer,GRect(-60+(int)dx,-60+(int)dy,290+(int)dx,290+(int)dy));//240->290
}


static void main_window_load(Window *window){
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
	// Create GBitmap
	s_bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MBG);
	s_out_ring_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OUT_RING);
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RING2);

	// Create BitmapLayer to display the GBitmap
	s_ring_parent_layer = bitmap_layer_create(GRect(-37,-37,216,216));
	s_bg_parent_layer = bitmap_layer_create(GRect(-60,-60,240,240));
	s_background_layer = bitmap_layer_create(bounds);

	// Set the bitmap onto the layer and add to the window
	
	//Rotating
	s_out_ring_layer = rot_bitmap_layer_create(s_out_ring_bitmap);
	rot_bitmap_set_compositing_mode(s_out_ring_layer,GCompOpSet);
	
	s_bg_layer = rot_bitmap_layer_create(s_bg_bitmap);
	rot_bitmap_set_compositing_mode(s_bg_layer,GCompOpSet);
	
	bitmap_layer_set_alignment(s_bg_parent_layer,GAlignTopLeft);
	bitmap_layer_set_alignment(s_ring_parent_layer,GAlignCenter);
	layer_add_child(bitmap_layer_get_layer(s_bg_parent_layer),(Layer *)s_bg_layer);
	layer_add_child(bitmap_layer_get_layer(s_ring_parent_layer),(Layer *)s_out_ring_layer);
	
	//Statics
	bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

	
	//Add Layers to window bottom-up
	layer_add_child(window_layer, bitmap_layer_get_layer(s_bg_parent_layer));
	layer_add_child(window_layer, bitmap_layer_get_layer(s_ring_parent_layer));
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
}
static void main_window_unload(Window *window){
	// Destroy GBitmap
	gbitmap_destroy(s_bg_bitmap);
	gbitmap_destroy(s_out_ring_bitmap);
	gbitmap_destroy(s_background_bitmap);
	// Destroy BitmapLayer
	rot_bitmap_layer_destroy(s_out_ring_layer);
	rot_bitmap_layer_destroy(s_bg_layer);
	bitmap_layer_destroy(s_bg_parent_layer);
	bitmap_layer_destroy(s_ring_parent_layer);
	bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void init(){
  //Create window element
  s_main_window = window_create();
	window_set_background_color(s_main_window, GColorPictonBlue);
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  // Make sure the time is displayed from the start
  update_time();
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  
}

static void deinit(){
  //Destroy window
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}