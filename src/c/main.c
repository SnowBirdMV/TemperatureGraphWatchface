#include <pebble.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
//0-19 temps
//20-39 pop data
//101 time since last fetch?
//101 humidity data
//102 condition string

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_battery_charge_layer;
static TextLayer *s_date_layer;
static TextLayer *s_steps_layer;
static TextLayer *s_cached_layer;
static TextLayer *s_refreshed_time_layer;
static TextLayer *s_data_refreshed_time_layer;
static Layer *s_graph_background;
static Layer *s_grid_background;
static Layer *s_battery_charge;
static Layer *s_battery_behind_clock;
static TextLayer *s_humidity_layer;

static BitmapLayer *s_background_layer;
static BitmapLayer *s_bluetooth_image;
static BitmapLayer *s_bluetooth_image_on;
static BitmapLayer *s_steps_above_layer;
static BitmapLayer *s_steps_below_layer;

static GBitmap *s_bluetooth_on;
static GBitmap *s_bluetooth_off;
static GBitmap *s_steps_above_image;
static GBitmap *s_steps_below_image;
static GBitmap *s_background_bitmap;

static GFont s_time_font;
static GFont s_battery_font;
static GFont s_weather_font;
static GFont s_date_font;
static GFont s_weather_font;

int tempData[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int popData[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int curent_humidity = 0;

unsigned long dataTime = 0;

void calculate_data_time_difference(){
    if (dataTime != 0){
    printf("Inside update_proc");
        static char refreshTimeBuffer[70];
        time_t curentTime = time(NULL);
        unsigned long curentTimeLong = curentTime;
        unsigned long timeDifference = curentTimeLong - dataTime;
        printf("Time Difference is: %lu, Curent Time is: %lu, Stored time is: %lu", timeDifference, curentTimeLong, dataTime);
        if (timeDifference < 61){
            snprintf(refreshTimeBuffer, sizeof(refreshTimeBuffer), "%lus", timeDifference);
        }
        else if (timeDifference > 61 && timeDifference < 3601){
            snprintf(refreshTimeBuffer, sizeof(refreshTimeBuffer), "%lum %lus", timeDifference / 60, timeDifference % 60);
        }
        else if (timeDifference > 3600 && timeDifference < 86400){
            snprintf(refreshTimeBuffer, sizeof(refreshTimeBuffer), "%luh %lum", timeDifference / 3600, timeDifference / 60 % 60);
        }
        else{
            snprintf(refreshTimeBuffer, sizeof(refreshTimeBuffer), "%lud %luh", timeDifference / 86400, timeDifference / 3600 % 24);
        }
    
        
        text_layer_set_text(s_refreshed_time_layer, refreshTimeBuffer );
        printf("Time sence last data refresh is: %s", refreshTimeBuffer);
        
        printf("Exiting update_proc");
    }
    else{
        text_layer_set_text(s_refreshed_time_layer, "" );
    }
}

static uint32_t const disconnect_vibe_segments[] = { 500, 250, 500 };
VibePattern disconnect_vibe = {
  .durations = disconnect_vibe_segments,
  .num_segments = ARRAY_LENGTH(disconnect_vibe_segments),
};

static uint32_t const connect_vibe_segments[] = { 250, 100, 250, 100, 250 };
VibePattern connect_vibe = {
  .durations = connect_vibe_segments,
  .num_segments = ARRAY_LENGTH(connect_vibe_segments),
};

static GPath *popPath = NULL;
static const GPathInfo popPathInfo = {
    .num_points = 22,
    .points = (GPoint []) {{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
                          {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
                          {160, 50}, {0, 50}}};

static int batteryCharge = 0;
static int s_step_count = 0, s_step_goal = 0, s_step_average = 0;


bool step_data_is_available() {
  return HealthServiceAccessibilityMaskAvailable &
    health_service_metric_accessible(HealthMetricStepCount,
      time_start_of_today(), time(NULL));
}

// Daily step goal
static void get_step_goal() {
  const time_t start = time_start_of_today();
  const time_t end = start + SECONDS_PER_DAY;
  s_step_goal = (int)health_service_sum_averaged(HealthMetricStepCount,
    start, end, HealthServiceTimeScopeDaily);
}

// Todays current step count
static void get_step_count() {
  s_step_count = (int)health_service_sum_today(HealthMetricStepCount);
}

// Average daily step count for this time of day
static void get_step_average() {
  const time_t start = time_start_of_today();
  const time_t end = time(NULL);
  s_step_average = (int)health_service_sum_averaged(HealthMetricStepCount,
    start, end, HealthServiceTimeScopeDaily);
}

static void update_step_average(){
    get_step_goal();
    get_step_count();
    get_step_average();
    printf("s_step_cpout: %d, s_step_average: %d, s_step_goal: %d", s_step_count, s_step_average, s_step_goal);
    if (s_step_average < s_step_count){
        layer_set_hidden(bitmap_layer_get_layer(s_steps_above_layer), false);
        layer_set_hidden(bitmap_layer_get_layer(s_steps_below_layer), true);
        printf("You are now above your step average");
    }
    else{
        layer_set_hidden(bitmap_layer_get_layer(s_steps_above_layer), true);
        layer_set_hidden(bitmap_layer_get_layer(s_steps_below_layer), false);
        printf("You are now below your step average");
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    printf("%s", "Starting to Parse Weather Data");
    //persist_write_int(103, time(NULL));
    time_t curentTime = time(NULL);
    printf("Storing Recieved Time: %" SCNd32 "\n", time(NULL));
    persist_write_data(101, &curentTime, sizeof(curentTime));
    dataTime = curentTime;
    
    time_t storedTime;
    unsigned long testLong;
    persist_read_data(101, &storedTime, sizeof(storedTime));
    testLong = storedTime;
    printf("unsigned long currently in storage is unsigned long: %lu", testLong);
    
  // Store incoming information
	static char temperature_buffer[8];
	static char conditions_buffer[32];
	static char weather_layer_buffer[32];
    static char humidity_buffer[10];
    static char pop_buffer[10];
    curent_humidity = (int)dict_find(iterator, 60)->value->int32;
    
    snprintf(humidity_buffer, sizeof(humidity_buffer), "%d%%", curent_humidity);
    text_layer_set_text(s_humidity_layer, humidity_buffer);
    persist_write_int(101, curent_humidity);
    
	Tuple* temps [20];
	Tuple* conds [20];
    Tuple* pops [20];
	char conditionStrings[32];
    conditionStrings[0] = conditionStrings[0];
	int tempInts[21];
	for (int i = 0; i < 21; i++){
		tempInts[i] = i;
	}
	for (int i = 0; i < 20; i++){
		pops[i] = dict_find(iterator, i);
		conds[i] = dict_find(iterator, i + 20);
        temps[i] = dict_find(iterator, i + 40);
	}
    
    
	for (int i = 0; i < 20; i++){
		snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)temps[i]->value->int32);
        snprintf(pop_buffer, sizeof(pop_buffer), "%d", (int)temps[i]->value->int32);
        popData[i] = (int)pops[i]->value->int32;
        tempInts[i] = (int)temps[i]->value->int32;
        tempData[i] = tempInts[i];
        persist_write_int(i, tempInts[i]);
        persist_write_int(i + 20, popData[i]);
        //int test = c.value;
        
	}
    
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conds[0]->value->cstring);
	conditionStrings[0] = conditions_buffer[0];
    printf("Condition Buffer: %s", conditions_buffer);

	static char tempDisplayBuffer[8];
	static char condDisplayBuffer[32];
	snprintf(tempDisplayBuffer, sizeof(tempDisplayBuffer), "%dF", (int)temps[0]->value->int32);
	snprintf(condDisplayBuffer, sizeof(condDisplayBuffer), "%s", conds[0]->value->cstring);
	snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", tempDisplayBuffer, condDisplayBuffer);
	text_layer_set_text(s_weather_layer, weather_layer_buffer);
    persist_write_string(102, weather_layer_buffer);
    layer_set_hidden(text_layer_get_layer(s_cached_layer), true);
    printf("%s", "Finished Parsing Weather Data");
    layer_mark_dirty(s_graph_background);
    calculate_data_time_difference();

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void update_time() {
  // Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
	static char s_buffer[8];
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
		"%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);
}
static void graph_bounds_layer_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here
	GRect graphLayerBounds = layer_get_frame(s_graph_background);
	GSize size = graphLayerBounds.size;
	int x2 = size.w;
	int y2 = size.h;
	int xDistance = x2/20;
    graphics_context_set_stroke_color(ctx, GColorBlue);
    graphics_context_set_fill_color(ctx, GColorBlue);
    int lastpoppoint = 50 - popData[0]/2;
    for (int i = 0; i < 20; i++){
        graphics_draw_line(ctx, GPoint(i * xDistance, lastpoppoint), GPoint((i + 1) * xDistance, 50 - popData[i]/2));
        popPathInfo.points[i] = GPoint((i + 1) * xDistance, 50 - popData[i]/2);
        lastpoppoint = 50 - popData[i]/2;
    }
    popPath = gpath_create(&popPathInfo);
    gpath_draw_filled(ctx, popPath);
    if (connection_service_peek_pebble_app_connection()){
        graphics_context_set_stroke_color(ctx, GColorGreen);
    }
    else{
        graphics_context_set_stroke_color(ctx, GColorRed);
    }
	graphics_draw_line(ctx, GPoint(0,0), GPoint(x2-1,0));
    //graphics_draw_line(ctx, GPoint(x2-1,0), GPoint(x2-1,y2-1));
	graphics_draw_line(ctx, GPoint(x2-1,y2-1), GPoint(0,y2-1));
    //graphics_draw_line(ctx, GPoint(0,y2-1), GPoint(0,0));
	int lastY = tempData[0]/2;
	graphics_context_set_stroke_color(ctx, GColorDarkGray);
	for (int i = 0; i < 20; i = i + 4){
		graphics_draw_line(ctx, GPoint((i + 1) * xDistance, y2), GPoint((i + 1) * xDistance, 0));
	}
	int mean = 0;
	int min = 9999;
	int max = -9999;
	for (int i = 0; i < 20; i++){
		if (tempData[i] < min){
			min = tempData[i];
		}
		if (tempData[i] > max){
			max = tempData[i];
		}
		mean = mean + tempData[i];
	}
	mean = mean / 20;
	printf("Min %d, Max %d, Mean %d", min, max, mean);
	int scale = ((y2/2)/(max-min));
	int offset = ((max - min + 2)/2) + (y2 / 2) - min;
	printf("Scaling the graph by %d, offsetting by %d test %d", scale, offset, (y2/(1000*(max-min))));
	graphics_context_set_stroke_color(ctx, GColorRed);
    int points[21];
    int diff1 = ((max - min) / 2) + min;
	diff1 = (diff1 + ((y2 / 2 - diff1) * 2.4) - 25) * -1;
	int pointY1= tempData[0] + ((y2 / 2 - tempData[0]) * 2.4);
	points[0] = pointY1 + diff1;
    lastY = points[0];
    for (int i = 0; i < 20; i++){
        int diff = ((max - min) / 2) + min;
		diff = (diff + ((y2 / 2 - diff) * 2.4) - 25) * -1;
		int pointY= tempData[i] + ((y2 / 2 - tempData[i]) * 2.4);
		int newPointY = pointY + diff;
        points[i] = newPointY;
    }
	for (int i = 0; i < 20; i++){
		//printf("newPointY %d, tempDatai %d, y2 %d, diff %d", points[i], tempData[i], y2, diffs[i]);
		graphics_draw_line(ctx, GPoint((i-1)*xDistance,lastY), GPoint(i*xDistance,points[i]));
		graphics_draw_line(ctx, GPoint((i-1)*xDistance,lastY+1), GPoint(i*xDistance,points[i]+1));
		if (i % 4 == 0){
            if (i != 20){
                static char graph_temp_buffer[10];
                snprintf(graph_temp_buffer, sizeof(graph_temp_buffer), "%d", tempData[i]);
                int shift = 0;
                int start = 0;
                if (i != 0){
                    if (points[i] >= 20){
                        while (start < points[i-1] && start < points[i+1] && start < points[i]){
                            start ++;
                            shift ++;
                        }
                        //printf("i is: %d, shift is: %d", i, shift);
                        graphics_draw_text(ctx, graph_temp_buffer, s_battery_font, GRect(i*xDistance-4, shift - 14 ,25, 18), GTextOverflowModeWordWrap, GTextAlignmentCenter, graphics_text_attributes_create());
                    }
                    else{
                        start = 50;
                        while (start > points[i-1] && start > points[i+1] && start > points[i]){
                            start --;
                            shift --;
                        }
                        //printf("i is: %d, shift is: %d", i, shift);
                        graphics_draw_text(ctx, graph_temp_buffer, s_battery_font, GRect(i*xDistance-4, 50 + shift,25, 18), GTextOverflowModeWordWrap, GTextAlignmentCenter, graphics_text_attributes_create());
                    }
                }
                else{
                    if (points[i] >= 20){
                        while (start < points[i+1] && start < points[i]){
                            start ++;
                            shift ++;
                        }
                        //printf("i is: %d, shift is: %d", i, shift);
                        graphics_draw_text(ctx, graph_temp_buffer, s_battery_font, GRect(i*xDistance-4, shift - 14 ,25, 18), GTextOverflowModeWordWrap, GTextAlignmentCenter, graphics_text_attributes_create());
                    }
                    else{
                        start = 50;
                        while (start > points[i+1] && start > points[i]){
                            start --;
                            shift --;
                        }
                        //printf("i is: %d, shift is: %d", i, shift);
                        graphics_draw_text(ctx, graph_temp_buffer, s_battery_font, GRect(i*xDistance-4, 50 + shift,25, 18), GTextOverflowModeWordWrap, GTextAlignmentCenter, graphics_text_attributes_create());
                    }
                }
                
            }	
		}
		lastY = points[i];
	}

	graphics_draw_line(ctx, GPoint((20)*xDistance,lastY), GPoint(x2,lastY));


}
static void grid_update_proc(Layer *layer, GContext *ctx){
    if (connection_service_peek_pebble_app_connection()){
        graphics_context_set_stroke_color(ctx, GColorGreen);
    }
    else{
        graphics_context_set_stroke_color(ctx, GColorRed);
    }
	graphics_draw_line(ctx, GPoint(0,40), GPoint(168,40));
	graphics_draw_line(ctx, GPoint(110,0), GPoint(110,40));
	graphics_draw_line(ctx, GPoint(110,26), GPoint(168,26));
    graphics_draw_line(ctx, GPoint(0,131), GPoint(168,131));
    graphics_draw_line(ctx, GPoint(92,131), GPoint(92,111));
    graphics_draw_line(ctx, GPoint(0,155), GPoint(168,155));
}

static void battery_charge_update_proc(Layer *layer, GContext *ctx){
    // Peek at the current battery state
	BatteryChargeState state = battery_state_service_peek();
    
	graphics_context_set_stroke_color(ctx, GColorGreen);
	graphics_context_set_fill_color(ctx, GColorGreen);
	graphics_fill_rect(ctx, GRect(5, 25, 10, -1*state.charge_percent*.2), 0, GCornerNone);
    
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(5,5), GPoint(5,25));
	graphics_draw_line(ctx, GPoint(5,25), GPoint(15,25));
	graphics_draw_line(ctx, GPoint(15,25), GPoint(15,5));
	graphics_draw_line(ctx, GPoint(15,5), GPoint(5,5));
	graphics_draw_line(ctx, GPoint(7,5), GPoint(7,3));
	graphics_draw_line(ctx, GPoint(7, 3), GPoint(13,3));
	graphics_draw_line(ctx, GPoint(13,3), GPoint(13,5));
	int batteryPercent = state.charge_percent;
	static char battery_buffer[100];
	snprintf(battery_buffer,sizeof(battery_buffer) , "%d%%", batteryPercent);
	printf("Battery Level is: %s", battery_buffer);
	text_layer_set_text(s_battery_charge_layer, battery_buffer );
	
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
    update_step_average();
  // Get weather update every 30 minutes
	if(tick_time->tm_min % 30 == 0) {
	// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

	// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

	// Send the message!
		app_message_outbox_send();
	}
}
static void battery_handler(BatteryChargeState charge){
	batteryCharge = charge.charge_percent;
}

static void app_connection_handler(bool connected) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Pebble app %sconnected", connected ? "" : "dis");
	if (connected){
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image_on), false);
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image), true);
		printf("%s", "Pebble Connected!");
        vibes_enqueue_custom_pattern(connect_vibe);
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image_on), true);
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image), false);
		printf("%s", "Pebble Disconnected!");
        vibes_enqueue_custom_pattern(disconnect_vibe);
	}
}

static void s_battery_behind_clock_update_proc(Layer *layer, GContext *ctx){
	BatteryChargeState state = battery_state_service_peek();

	graphics_context_set_stroke_color(ctx, GColorGreen);
	graphics_context_set_fill_color(ctx, GColorDarkGreen );
	graphics_fill_rect(ctx, GRect(0, 0, state.charge_percent*1.1, 40), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorDarkCandyAppleRed);
	graphics_fill_rect(ctx, GRect(state.charge_percent*1.1, 0, 110-state.charge_percent*1.1, 40), 0, GCornerNone);
}
static void s_date_layer_update_proc(Layer *layer, GContext *ctx){
	time_t rawtime;
	struct tm *info;
	char date_buffer[80];
	time( &rawtime );
	info = localtime( &rawtime );
	strftime(date_buffer,80,"%a, %d/%m/%y", info);
	printf("Formatted date & time : |%s|\n", date_buffer );
	text_layer_set_text(s_date_layer, date_buffer );
}

static void kit_connection_handler(bool connected) {
	APP_LOG(APP_LOG_LEVEL_INFO, "PebbleKit %sconnected", connected ? "" : "dis");
}

static void health_handler(HealthEventType event, void *context) {
  // Which type of event occurred?
  switch(event) {
    case HealthEventSignificantUpdate:
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventSignificantUpdate event");
      update_step_average();
      
      break;
    case HealthEventMovementUpdate:
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventMovementUpdate event");
      update_step_average();
      break;
    case HealthEventSleepUpdate:
      APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventSleepUpdate event");
      update_step_average();
      break;
    case HealthEventHeartRateUpdate:
      break;
    case HealthEventMetricAlert:
      break;
  }
}

static void update_humidity(){
    static char humidity_buffer[10];
    snprintf(humidity_buffer, sizeof(humidity_buffer), "%d%%", curent_humidity);
    text_layer_set_text(s_humidity_layer, humidity_buffer);
}



static void main_window_load(Window *window) {
  // Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
    printf("%"PRIu32 , persist_read_int(0));
    for (int i = 0; i < 20; i++){
        tempData[i] = persist_read_int(i);
        popData[i] = persist_read_int(i + 20);
    }
    curent_humidity = persist_read_int(101);
    
    printf("Curent Humidity Cached: %" SCNd32 "\n", persist_read_int(101));
    
    get_step_average();
    get_step_count();
    get_step_goal();
    
    // Create GFont
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIGITAL_FONT_48));
	s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
	s_battery_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    s_weather_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
	

	s_battery_behind_clock = layer_create(GRect(0, 0, 110, 40));
	layer_set_update_proc(s_battery_behind_clock, s_battery_behind_clock_update_proc);
	layer_add_child(window_layer,s_battery_behind_clock);

  // Create GBitmap
  //s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

  // Create BitmapLayer to display the GBitmap
  //s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  //bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  //layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Create the TextLayer with specific bounds
	s_time_layer = text_layer_create(
		GRect(1, PBL_IF_ROUND_ELSE(0, -13), 110, 50));

  // Improve the layout to be more like a watchface
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_text(s_time_layer, "00:00");
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Apply to TextLayer
	text_layer_set_font(s_time_layer, s_time_font);



  // Create temperature Layer
	s_weather_layer = text_layer_create(
		GRect(0, 126, bounds.size.w, 30));


  // Style the text
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_text_color(s_weather_layer, GColorWhite);
	text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
    static char weather_layer_buffer[100];
    persist_read_string(102, weather_layer_buffer, 100);
    printf("Character buffer0 is %s", weather_layer_buffer);

  // Create second custom font, apply it and add to Window
	s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
	text_layer_set_font(s_weather_layer, s_date_font);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
	layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
    
    s_cached_layer = text_layer_create(GRect(0, 153, 144, 25));
    text_layer_set_background_color(s_cached_layer, GColorClear);
	text_layer_set_text_color(s_cached_layer, GColorWhite);
	text_layer_set_text_alignment(s_cached_layer, GTextAlignmentCenter);
    text_layer_set_font(s_cached_layer, s_battery_font);
    text_layer_set_text(s_cached_layer, "Cached");
    layer_set_hidden(text_layer_get_layer(s_cached_layer), false);
    layer_add_child(window_layer,text_layer_get_layer(s_cached_layer));

	s_graph_background = layer_create(bounds);
	layer_set_frame(s_graph_background, GRect(-1, 61, 160, 50));
	layer_set_update_proc(s_graph_background, graph_bounds_layer_update_proc);
	layer_add_child(window_layer, s_graph_background);

	s_grid_background = layer_create(bounds);
	layer_set_update_proc(s_grid_background, grid_update_proc);
	layer_add_child(window_layer,s_grid_background);

	s_bluetooth_off = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_OFF);
	s_bluetooth_image = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_bluetooth_image, s_bluetooth_off);
	layer_set_frame(bitmap_layer_get_layer(s_bluetooth_image), GRect(123, -1, 26, 26));
	layer_add_child(window_layer,bitmap_layer_get_layer(s_bluetooth_image));

	s_bluetooth_on = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ON);
	s_bluetooth_image_on = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_bluetooth_image_on, s_bluetooth_on);
	layer_set_frame(bitmap_layer_get_layer(s_bluetooth_image_on), GRect(123, -1, 26, 26));
	layer_add_child(window_layer,bitmap_layer_get_layer(s_bluetooth_image_on));

	bool app_connection = connection_service_peek_pebble_app_connection();
	if (app_connection){
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image_on), false);
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image), true);
		printf("%s", "Pebble Connected!");
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image_on), true);
		layer_set_hidden(bitmap_layer_get_layer(s_bluetooth_image), false);
		printf("%s", "Pebble Disconnected!");
	}


	s_battery_charge = layer_create(bounds);
	layer_set_update_proc(s_battery_charge, battery_charge_update_proc);
	layer_set_frame(s_battery_charge, GRect(110, -2, 20, 40));
	layer_add_child(window_layer,s_battery_charge);


	s_battery_charge_layer = text_layer_create(GRect(113, 24, 30, 40));
	text_layer_set_font(s_battery_charge_layer, s_battery_font);
	text_layer_set_background_color(s_battery_charge_layer, GColorClear);
	text_layer_set_text_color(s_battery_charge_layer, GColorWhite);
	text_layer_set_text_alignment(s_battery_charge_layer, GTextAlignmentCenter);
	layer_add_child(window_layer,text_layer_get_layer(s_battery_charge_layer));


	s_date_layer = text_layer_create(GRect(0, 34, 144, 25));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	text_layer_set_font(s_date_layer, s_date_font);
	layer_add_child(window_layer,text_layer_get_layer(s_date_layer));
    
    s_steps_layer = text_layer_create(GRect(20, 105, 40, 25));
    text_layer_set_background_color(s_steps_layer, GColorClear);
	text_layer_set_text_color(s_steps_layer, GColorWhite);
	text_layer_set_text_alignment(s_steps_layer, GTextAlignmentLeft);
    text_layer_set_font(s_steps_layer, s_date_font);
    static char stepBuffer[10];
    snprintf(stepBuffer, sizeof(stepBuffer), "%d", (int)health_service_sum_today(HealthMetricStepCount));
    text_layer_set_text(s_steps_layer, stepBuffer);
    layer_add_child(window_layer,text_layer_get_layer(s_steps_layer));
    
	time_t rawtime;
	struct tm *info;
	char date_buffer[80];
	time( &rawtime );
	info = localtime( &rawtime );
	strftime(date_buffer,80,"%a, %d/%m/%y", info);
	printf("Formatted date & time : |%s|\n", date_buffer );
	text_layer_set_text(s_date_layer, date_buffer );
    
    s_steps_above_image = gbitmap_create_with_resource(RESOURCE_ID_STEPS_ABOVE);
	s_steps_above_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_steps_above_layer, s_steps_above_image);
	layer_set_frame(bitmap_layer_get_layer(s_steps_above_layer), GRect(0, 111, 20, 20));
	layer_add_child(window_layer,bitmap_layer_get_layer(s_steps_above_layer));
    
    s_steps_below_image = gbitmap_create_with_resource(RESOURCE_ID_STEPS_BELOW);
	s_steps_below_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_steps_below_layer, s_steps_below_image);
	layer_set_frame(bitmap_layer_get_layer(s_steps_below_layer), GRect(0, 111, 20, 20));
	layer_add_child(window_layer,bitmap_layer_get_layer(s_steps_below_layer));
    
    update_step_average();
    
    s_humidity_layer = text_layer_create(GRect(95, 105, 40, 25));
    text_layer_set_background_color(s_humidity_layer, GColorClear);
	text_layer_set_text_color(s_humidity_layer, GColorWhite);
	text_layer_set_text_alignment(s_humidity_layer, GTextAlignmentLeft);
    text_layer_set_font(s_humidity_layer, s_date_font);
    layer_add_child(window_layer,text_layer_get_layer(s_humidity_layer));
    text_layer_set_overflow_mode(s_humidity_layer, GTextOverflowModeWordWrap);
    update_humidity();
    
    
    s_refreshed_time_layer = text_layer_create(GRect(5, 153, 140, 25));
    text_layer_set_background_color(s_refreshed_time_layer, GColorClear);
	text_layer_set_text_color(s_refreshed_time_layer, GColorWhite);
	text_layer_set_text_alignment(s_refreshed_time_layer, GTextAlignmentLeft);
    text_layer_set_overflow_mode(s_refreshed_time_layer, GTextOverflowModeWordWrap);
    text_layer_set_font(s_refreshed_time_layer, s_battery_font);
    text_layer_set_text(s_refreshed_time_layer, "Hello" );
    layer_add_child(window_layer,text_layer_get_layer(s_refreshed_time_layer));
    calculate_data_time_difference();
    
    
    
    


	connection_service_subscribe((ConnectionHandlers) {
		.pebble_app_connection_handler = app_connection_handler,
		.pebblekit_connection_handler = kit_connection_handler
	});
	//printf("%s", "After registering for connection service");
    
    
    #if defined(PBL_HEALTH)
    health_service_events_subscribe(health_handler, NULL);
    if(!health_service_events_subscribe(health_handler, NULL)) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    }
    #else
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    #endif

    
	battery_state_service_subscribe(battery_handler);
  // Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    printf("%s", "Registered last window");

}

static void main_window_unload(Window *window) {
	layer_destroy(s_graph_background);
  // Destroy TextLayer
	text_layer_destroy(s_time_layer);

  // Unload GFont
	fonts_unload_custom_font(s_time_font);

  // Destroy GBitmap
	gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
	bitmap_layer_destroy(s_background_layer);

  // Destroy weather elements
	text_layer_destroy(s_weather_layer);
}


static void init() {
  // Create main Window element and assign to pointer
	s_main_window = window_create();

  // Set the background color
	window_set_background_color(s_main_window, GColorBlack);

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
	time_t rawtime;
	struct tm *info;
	char date_buffer[80];
	time( &rawtime );
	info = localtime( &rawtime );
	strftime(date_buffer,80,"%a, %d/%m/%y", info);
	printf("Formatted date & time : |%s|\n", date_buffer );
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text(s_date_layer, date_buffer );
	
    printf("%s", "Registering callbacks");
    // Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
    printf("%s", "Finished Registering callbacks");

    // Open AppMessage
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    printf("%s", "Opened AppMessage");
}

static void deinit() {
  // Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
    printf("%s", "init-ing");
	init();
    printf("%s", "app looping");
	app_event_loop();
    printf("%s", "de-initing");
	deinit();
    printf("%s", "de-init-ed");
}