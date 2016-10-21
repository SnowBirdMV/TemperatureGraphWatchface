#include <pebble.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define KEY_CONDITIONS 1

#define KEY_TEMPERATURE 100
#define KEY_POPDATA 200
#define KEY_WINDDATA 300
#define KEY_HUMIDITY 1000
#define KEY_TIME 1001
#define KEY_WEATHER_STRING 1002
#define KEY_BATTERY_TIME 1003
#define KEY_BACKGROUNDCOLOR 1004
#define KEY_GRID_COLOR 1005
#define KEY_SECOND_TICK 1006 
//todo remove key_second_tick
#define KEY_ANIMATIONS 1007
#define KEY_INBOX_HANDLER 10000
#define KEY_JS_READY 10001
#define KEY_BATTERY_COLOR 1008
#define KEY_TEMP_COLOR 1009
#define KEY_POP_COLOR 1010
#define KEY_BATTERY_UNCHARGED 1011
#define KEY_TIME_COLOR 1012
#define KEY_DATE_COLOR 1013
#define KEY_BATTERY_PERCENTAGE 1014
#define KEY_TEMP_NUMBER_COLOR 1015
#define KEY_STEPS_COLOR 1016
#define KEY_DATA_TIME_COLOR 1017
#define KEY_BATTERY_LIFE_COLOR 1018
#define KEY_HUMIDITY_COLOR 1019
#define KEY_WEATHER_COLOR 1020
#define KEY_GRAPH_LINE_COLOR 1021
#define KEY_BATTERY_OUTLINE_COLOR 1022
#define KEY_BATTERY_BEHIND_CLOCK_TOGGLE 1023
#define KEY_SECONDS_TICK 1024
#define KEY_SECONDS_COUNT 1025
#define KEY_SLEEP_MODE_TOGGLE 1032
#define KEY_WIND_SPEED_CURENT 1033 
#define KEY_CURENT_WIND_DIRECTION 1034 
#define KEY_GRID_COLOR_CHANGE_TOGGLE 1031
#define KEY_CELCIUS_TOGGLE 1035
#define KEY_TRIPLE_SHAKE 1036
#define KEY_BOTTEM_LEFT 1037
#define KEY_BOTTEM_RIGHT 1038

#define TEMP_DATA_POINTS 20
#define POP_DATA_POINTS 20
#define WIND_DATA_POINTS 20


static Window *s_main_window;
static Window *s_sleep_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_battery_charge_layer;
static TextLayer *s_date_layer;
static TextLayer *s_steps_layer;
static TextLayer *s_bottem_left_layer;
static TextLayer *s_bottem_right_layer;
static TextLayer *s_data_refreshed_time_layer;
static TextLayer *s_battery_time_layer;
static TextLayer *s_sleep_layer;
static TextLayer *s_humidity_layer;
static TextLayer *s_bgcolor_layer;
static TextLayer *s_seconds_layer;
static TextLayer *s_wind_speed_layer;
static Layer *s_graph_background;
static Layer *s_grid_background;
static Layer *s_battery_charge;
static Layer *s_battery_behind_clock;



static BitmapLayer *s_background_layer;
static BitmapLayer *s_bluetooth_image;
static BitmapLayer *s_bluetooth_image_on;
static BitmapLayer *s_steps_above_layer;
static BitmapLayer *s_steps_below_layer;
static BitmapLayer *s_battery_charging_layer;

static GBitmap *s_bluetooth_on;
static GBitmap *s_bluetooth_off;
static GBitmap *s_steps_above_image;
static GBitmap *s_steps_below_image;
static GBitmap *s_background_bitmap;
static GBitmap *s_lightningbolt;

static GFont s_time_font;
static GFont s_battery_font;
static GFont s_weather_font;
static GFont s_date_font;
static GFont s_weather_font;

GColor bg_color;
GColor grid_color;
GColor battery_color;
GColor temp_color;
GColor pop_color;
GColor uncharged_color;
GColor time_color;
GColor date_color;
GColor battery_percentage_color;
GColor temp_number_color;
GColor steps_color;
GColor data_time_color;
GColor battery_life_color;
GColor weather_color;
GColor humidity_color;
GColor graph_line_color;
GColor battery_outline_color;

bool asleep = false;
bool bat_behind_graph;
bool show_seconds;
bool sleep_mode_enabled;
bool change_grid_color;
bool triple_shake;
bool celcius;

int asleep_time = 0;
int second_counter = 0;
int num_seconds;
int accelBuffer = 0;

static char reasonStr[20];
static char bottemLeft[30];
static char bottemRight[30];
static char calorie_count_str[10];
static char sleep_time_string[15];

int tempData[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int popData[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int curent_humidity = 0;

unsigned long dataTime = 0;
static bool s_js_ready = true;

bool comm_is_js_ready() {
	return s_js_ready;
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
	.num_points = 23,
	.points = (GPoint []) {{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, 
	{160, 50}, {0, 50}, {0,0}}};



static void storeOptions(){
	//colors
	persist_read_data(KEY_BACKGROUNDCOLOR, &bg_color, sizeof(bg_color));
	persist_read_data(KEY_GRID_COLOR, &grid_color, sizeof(grid_color));
	persist_read_data(KEY_BATTERY_COLOR, &battery_color, sizeof(battery_color));
	persist_read_data(KEY_TEMP_COLOR, &temp_color, sizeof(temp_color));
	persist_read_data(KEY_POP_COLOR, &pop_color, sizeof(pop_color));
	persist_read_data(KEY_BATTERY_UNCHARGED, &uncharged_color, sizeof(uncharged_color));
	persist_read_data(KEY_TIME_COLOR, &time_color, sizeof(time_color));
	persist_read_data(KEY_DATE_COLOR, &date_color, sizeof(date_color));
	persist_read_data(KEY_BATTERY_PERCENTAGE, &battery_percentage_color, sizeof(battery_percentage_color));
	persist_read_data(KEY_TEMP_NUMBER_COLOR, &temp_number_color, sizeof(temp_number_color));
	persist_read_data(KEY_STEPS_COLOR, &steps_color, sizeof(steps_color));
	persist_read_data(KEY_DATA_TIME_COLOR, &data_time_color, sizeof(data_time_color));
	persist_read_data(KEY_BATTERY_LIFE_COLOR, &battery_life_color, sizeof(battery_life_color));
	persist_read_data(KEY_HUMIDITY_COLOR, &humidity_color, sizeof(humidity_color));
	persist_read_data(KEY_WEATHER_COLOR, &weather_color, sizeof(weather_color));
	persist_read_data(KEY_GRAPH_LINE_COLOR, &graph_line_color, sizeof(graph_line_color));
	persist_read_data(KEY_BATTERY_OUTLINE_COLOR, &battery_outline_color, sizeof(battery_outline_color));
	
	//booleans
	persist_read_data(KEY_BATTERY_BEHIND_CLOCK_TOGGLE, &bat_behind_graph, sizeof(bat_behind_graph));
	persist_read_data(KEY_SECONDS_TICK, &show_seconds, sizeof(show_seconds));
	persist_read_data(KEY_SLEEP_MODE_TOGGLE, &sleep_mode_enabled, sizeof(sleep_mode_enabled));
	persist_read_data(KEY_GRID_COLOR_CHANGE_TOGGLE, &change_grid_color, sizeof(change_grid_color));
	persist_read_data(KEY_TRIPLE_SHAKE, &triple_shake, sizeof(triple_shake));
	persist_read_data(KEY_CELCIUS_TOGGLE, &celcius, sizeof(celcius));
	
	//strings
	persist_read_string(KEY_BOTTEM_LEFT, bottemLeft, sizeof(bottemLeft));
	persist_read_string(KEY_BOTTEM_RIGHT, bottemRight, sizeof(bottemRight));
	
	//integers
	persist_read_data(KEY_SECONDS_COUNT, &num_seconds, sizeof(num_seconds));

}

const char*  calculate_data_time_difference(){
	time_t storedTime;
	unsigned long testLong;
	persist_read_data(KEY_TIME, &storedTime, sizeof(storedTime));
	testLong = (unsigned long)storedTime;
	//printf("Time currently in storage (from loading main window) is : %lu", (unsigned long)testLong);
	dataTime = testLong;
	static char refreshTimeBuffer[70];
	if (dataTime != 0){
		//printf("Inside update_proc");
		
		time_t curentTime = time(NULL);
		unsigned long curentTimeLong = curentTime;
		unsigned long timeDifference = curentTimeLong - dataTime;
		//printf("Time Difference is: %lu, Curent Time is: %lu, Stored time is: %lu", timeDifference, curentTimeLong, dataTime);
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
		//text_layer_set_text_color(s_bottem_left_layer, data_time_color);
		//text_layer_set_text(s_bottem_left_layer, refreshTimeBuffer );
		printf("Time sence last data refresh is: %s", refreshTimeBuffer);
		
		
		//printf("Exiting update_proc");
	}
	else{
		text_layer_set_text(s_bottem_left_layer, "Loading" );
	}
	return refreshTimeBuffer;
}

const char* calculate_battery_time_difference(){
	time_t storedBatteryTime;
	unsigned long testLong;
	persist_read_data(KEY_BATTERY_TIME, &storedBatteryTime, sizeof(storedBatteryTime));
	testLong = (unsigned long)storedBatteryTime;
	//printf("Time currently in storage (from loading main window) is : %lu", (unsigned long)testLong);
	static char refreshTimeBuffer[70];
	time_t batteryTime = testLong;
	if ((unsigned long)storedBatteryTime != 0){
		//printf("Inside update_proc");
		
		time_t curentTime = time(NULL);
		unsigned long curentTimeLong = curentTime;
		unsigned long timeDifference = curentTimeLong - (unsigned long)storedBatteryTime;
		//printf("Time Difference is: %lu, Curent Time is: %lu, Stored time is: %lu", timeDifference, curentTimeLong, (unsigned long)storedBatteryTime);
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
		
		//text_layer_set_text(s_battery_time_layer, "FIX ME" );
		printf("Time sence last charge is: %s", refreshTimeBuffer);
		
		
		//printf("Exiting update_proc");
	}
	else{
		text_layer_set_text(s_bottem_left_layer, "Loading" );
	}
	return refreshTimeBuffer;
}


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

static void update_calorie_count(){
	HealthMetric resting = HealthMetricRestingKCalories;
	HealthMetric active = HealthMetricActiveKCalories;
	int totalCalories = resting + active;
	snprintf(calorie_count_str, sizeof(calorie_count_str), "%d", totalCalories);
	printf("Total calories is: %d", totalCalories);
	
}

static void update_sleep_time(){
	HealthMetric sleepTimeSeconds = HealthMetricSleepSeconds;
	unsigned long sleepTime = (unsigned long)sleepTimeSeconds;
	if (sleepTime < 61){
			snprintf(sleep_time_string, sizeof(sleep_time_string), "%lus", sleepTime);
		}
		else if (sleepTime > 61 && sleepTime < 3601){
			snprintf(sleep_time_string, sizeof(sleep_time_string), "%lum %lus", sleepTime / 60, sleepTime % 60);
		}
		else if (sleepTime > 3600 && sleepTime < 86400){
			snprintf(sleep_time_string, sizeof(sleep_time_string), "%luh %lum", sleepTime / 3600, sleepTime / 60 % 60);
		}
		else{
			snprintf(sleep_time_string, sizeof(sleep_time_string), "%lud %luh", sleepTime / 86400, sleepTime / 3600 % 24);
		}
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
	graphics_context_set_stroke_color(ctx, pop_color);
	graphics_context_set_fill_color(ctx, pop_color);
	int lastpoppoint = 50 - popData[0]/2;
	for (int i = 0; i < 20; i++){
		graphics_draw_line(ctx, GPoint(i * xDistance, lastpoppoint), GPoint((i + 1) * xDistance, 50 - popData[i]/2));
		popPathInfo.points[i] = GPoint((i + 1) * xDistance, 50 - popData[i]/2);
		lastpoppoint = 50 - popData[i]/2;
	}
	popPathInfo.points[22] = GPoint(0, 50 - popData[0]/2);
	popPath = gpath_create(&popPathInfo);
	gpath_draw_filled(ctx, popPath);
	if(change_grid_color){
	if (connection_service_peek_pebble_app_connection()){
		graphics_context_set_stroke_color(ctx, grid_color);
	}
	else{
		graphics_context_set_stroke_color(ctx, GColorRed);
	}
	}
	else{
		graphics_context_set_stroke_color(ctx, grid_color);
	}
	graphics_draw_line(ctx, GPoint(0,0), GPoint(x2-1,0));
	//graphics_draw_line(ctx, GPoint(x2-1,0), GPoint(x2-1,y2-1));
	graphics_draw_line(ctx, GPoint(x2-1,y2-1), GPoint(0,y2-1));
	//graphics_draw_line(ctx, GPoint(0,y2-1), GPoint(0,0));
	int lastY = tempData[0]/2;
	graphics_context_set_stroke_color(ctx, graph_line_color);
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
	//printf("Min %d, Max %d, Mean %d", min, max, mean);
	int scale = ((y2/2)/(max-min));
	int offset = ((max - min + 2)/2) + (y2 / 2) - min;
	//printf("Scaling the graph by %d, offsetting by %d test %d", scale, offset, (y2/(1000*(max-min))));
	graphics_context_set_stroke_color(ctx, temp_color);
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
		graphics_context_set_text_color(ctx, temp_number_color);
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
	if(change_grid_color){
	if (connection_service_peek_pebble_app_connection()){
		graphics_context_set_stroke_color(ctx, grid_color);
	}
	else{
		graphics_context_set_stroke_color(ctx, GColorRed);
	}
	}
	else{
		graphics_context_set_stroke_color(ctx, grid_color);
	}
	graphics_draw_line(ctx, GPoint(0,40), GPoint(168,40));
	graphics_draw_line(ctx, GPoint(110,0), GPoint(110,40));
	graphics_draw_line(ctx, GPoint(110,26), GPoint(168,26));
	graphics_draw_line(ctx, GPoint(0,131), GPoint(168,131));
	graphics_draw_line(ctx, GPoint(92,131), GPoint(92,111));
	graphics_draw_line(ctx, GPoint(0,155), GPoint(168,155));
	graphics_draw_line(ctx, GPoint(50,155), GPoint(50,170));
	graphics_draw_line(ctx, GPoint(92,155), GPoint(92,170));
}

static void battery_charge_update_proc(Layer *layer, GContext *ctx){
	// Peek at the current battery state
	BatteryChargeState state = battery_state_service_peek();

	if(state.is_charging){
		time_t curentTime = time(NULL);
		persist_write_data(KEY_BATTERY_TIME, &curentTime, sizeof(curentTime));
	}

	graphics_context_set_stroke_color(ctx, battery_color);
	graphics_context_set_fill_color(ctx, battery_color);
	graphics_fill_rect(ctx, GRect(5, 25, 10, -1*state.charge_percent*.2), 0, GCornerNone);

	graphics_context_set_stroke_color(ctx, battery_outline_color);
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
	text_layer_set_text_color(s_battery_charge_layer, battery_percentage_color);
	text_layer_set_text(s_battery_charge_layer, battery_buffer );

}
static void updateWeather(){
	// Get weather update every hour
	time_t storedTime;
	persist_read_data(KEY_TIME, &storedTime, sizeof(storedTime));
	if((unsigned long)time(NULL) - (unsigned long)storedTime > 3599 && comm_is_js_ready()) {
		printf("Time difference is %lu, time1: %lu, time2: %lu", (unsigned long)time(NULL) - (unsigned long)storedTime, (unsigned long)time(NULL), (unsigned long)storedTime);
			// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

			// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

			// Send the message!
		printf("Sent request for weather to phone.");
		app_message_outbox_send();
	}
}

static void forceWeatherUpdate(){
	//still checks if javascript is ready
	if (comm_is_js_ready()){
	DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

			// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

			// Send the message!
		printf("Sent request for weather to phone.");
		app_message_outbox_send();
	}
	else{
		printf("Tried to send wather data but JS was not ready");
	}
}

static void battery_handler(BatteryChargeState charge){
	batteryCharge = charge.charge_percent;
	if (charge.is_charging){
		layer_set_hidden(bitmap_layer_get_layer(s_battery_charging_layer), false);
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(s_battery_charging_layer), true);
	}
	if(charge.is_charging){
		time_t batteryChargeTime = time(NULL);
		persist_write_data(KEY_BATTERY_TIME, &batteryChargeTime, sizeof(batteryChargeTime));
	}
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

	graphics_context_set_stroke_color(ctx, battery_color);
	graphics_context_set_fill_color(ctx, battery_color );
	graphics_fill_rect(ctx, GRect(0, 0, state.charge_percent*1.1, 40), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, uncharged_color);
	graphics_fill_rect(ctx, GRect(state.charge_percent*1.1, 0, 110-state.charge_percent*1.1, 40), 0, GCornerNone);
}
static void s_date_layer_update_proc(Layer *layer, GContext *ctx){
	time_t rawtime;
	struct tm *info;
	static char date_buffer[80];
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
		update_calorie_count();
		break;
		case HealthEventSleepUpdate:
		APP_LOG(APP_LOG_LEVEL_INFO, 
			"New HealthService HealthEventSleepUpdate event");
		update_sleep_time();
		if (sleep_mode_enabled){
			asleep = true;
			asleep_time = 10;
		}
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
	text_layer_set_text_color(s_humidity_layer, humidity_color);
	text_layer_set_text(s_humidity_layer, humidity_buffer);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
	if(triple_shake){
	if (accelBuffer > 6){
		printf("Forced a weather update");
		forceWeatherUpdate();
		accelBuffer = 0;
	}
	else{
		accelBuffer = accelBuffer + 3;
	}
	}
	if(show_seconds){
		second_counter = num_seconds;
	}
	asleep_time = 0;
	asleep = false;
	if (window_stack_get_top_window() == s_sleep_window){
		window_stack_pop(true);
	}
	update_time();
	update_step_average();
	calculate_data_time_difference();
	calculate_battery_time_difference();
	update_step_average();
	time_t storedTime;
	persist_read_data(KEY_TIME, &storedTime, sizeof(storedTime));
	if((unsigned long)time(NULL) - (unsigned long)storedTime > 3599) {
		printf("Time difference is %lu, time1: %lu, time2: %lu", (unsigned long)time(NULL) - (unsigned long)storedTime, (unsigned long)time(NULL), (unsigned long)storedTime);
	// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

	// Add a key-value pair
		dict_write_uint8(iter, 0, 0);

	// Send the message!
		printf("Sent request for weather to phone.");
		app_message_outbox_send();
	}
}
static void seconds_proc(Layer *layer, GContext *ctx){

}

static void lightningbolt_update_proc(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_lightningbolt, gbitmap_get_bounds(s_lightningbolt));
}

static void bluetooth_on_proc(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_bluetooth_on, gbitmap_get_bounds(s_bluetooth_on));
}
static void bluetooth_off_proc(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_bluetooth_off, gbitmap_get_bounds(s_bluetooth_off));
}

static void steps_above_update_proc(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_steps_above_image, gbitmap_get_bounds(s_steps_above_image));
}
static void steps_below_update_proc(Layer *layer, GContext *ctx){
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_steps_below_image, gbitmap_get_bounds(s_steps_below_image));
}
static void updateWindSpeed(){
    static char windDirectionString[5];
    int windDirection = persist_read_int(KEY_CURENT_WIND_DIRECTION);
    if (windDirection >= 348 || windDirection < 11){
        snprintf(windDirectionString, sizeof(windDirectionString), "N");
    }
    else if (windDirection >= 11 || windDirection < 33){
        snprintf(windDirectionString, sizeof(windDirectionString), "NNE");
    }
    else if (windDirection >= 33 || windDirection < 56){
        snprintf(windDirectionString, sizeof(windDirectionString), "NE");
    }
    else if (windDirection >= 56 || windDirection < 78){
        snprintf(windDirectionString, sizeof(windDirectionString), "ENE");
    }
    else if (windDirection >= 78 || windDirection < 101){
        snprintf(windDirectionString, sizeof(windDirectionString), "E");
    }
    else if (windDirection >= 101 || windDirection < 123){
        snprintf(windDirectionString, sizeof(windDirectionString), "ESE");
    }
    else if (windDirection >= 123 || windDirection < 146){
        snprintf(windDirectionString, sizeof(windDirectionString), "SE");
    }
    else if (windDirection >= 146 || windDirection < 168){
        snprintf(windDirectionString, sizeof(windDirectionString), "SSE");
    }
    else if (windDirection >= 168 || windDirection < 191){
        snprintf(windDirectionString, sizeof(windDirectionString), "S");
    }
    else if (windDirection >= 191 || windDirection < 213){
        snprintf(windDirectionString, sizeof(windDirectionString), "SSW");
    }
    else if (windDirection >= 213 || windDirection < 236){
        snprintf(windDirectionString, sizeof(windDirectionString), "SW");
    }
    else if (windDirection >= 236 || windDirection < 258){
        snprintf(windDirectionString, sizeof(windDirectionString), "WSW");
    }
    else if (windDirection >= 258 || windDirection < 281){
        snprintf(windDirectionString, sizeof(windDirectionString), "W");
    }
    else if (windDirection >= 281 || windDirection < 303){
        snprintf(windDirectionString, sizeof(windDirectionString), "WNW");
    }
    else if (windDirection >= 303 || windDirection < 326){
        snprintf(windDirectionString, sizeof(windDirectionString), "NW");
    }
    else if (windDirection >= 326 || windDirection < 348){
        snprintf(windDirectionString, sizeof(windDirectionString), "NNW");
    }
    
    
    int windData = persist_read_int(KEY_WIND_SPEED_CURENT);
    static char wind_buffer[10];
	snprintf(wind_buffer, sizeof(wind_buffer), "%s %d", windDirectionString, windData);
	text_layer_set_text_color(s_wind_speed_layer, GColorWhite);
	text_layer_set_text(s_wind_speed_layer, wind_buffer);
}


static void weather_update_time_update_proc(Layer *layer, GContext *ctx){
	printf("Registered weather update time");
}
static void calories_burned_update_proc(Layer *layer, GContext *ctx){
	printf("Registered calories burned");
}
static void time_slept_update_proc(Layer *layer, GContext *ctx){
	printf("Registered tiem slept");
}
static void battery_charge_time_update_proc(Layer *layer, GContext *ctx){
	printf("Registered battery charge time");
}



//-----------------------------------------------------------------------------------------------------
//									Update Bottem Left
//-----------------------------------------------------------------------------------------------------

static void update_bot_left(){
	printf("Ditermining bot left setting");
	if (strcmp(bottemLeft, "WeatherUpdateTime") == 0){
		printf("bot_left setting to: WeatherUpdateTime");
		text_layer_set_text(s_bottem_left_layer, calculate_data_time_difference());
	}
	else if (strcmp(bottemLeft, "CaloriesBurned") == 0){
		printf("bot_left setting to: CaloriesBurned");
		text_layer_set_text(s_bottem_left_layer, calorie_count_str);
		//layer_set_update_proc(text_layer_get_layer(s_bottem_left_layer), calories_burned_update_proc);
	}
	else if (strcmp(bottemLeft, "TimeSlept") == 0){
		printf("bot_left setting to: TimeSlept");
		text_layer_set_text(s_bottem_left_layer, sleep_time_string);
		//layer_set_update_proc(text_layer_get_layer(s_bottem_left_layer), time_slept_update_proc);
	}
	else if (strcmp(bottemLeft, "BatteryChargeTime") == 0){
		printf("bot_left setting to: BatteryChargeTime");
		text_layer_set_text(s_bottem_left_layer, calculate_battery_time_difference());
		
		//layer_set_update_proc(text_layer_get_layer(s_bottem_left_layer), battery_charge_time_update_proc);
	}
	else{
		printf("Invalid bottem left Identifier");
		printf("Recieved: %s", bottemLeft);
	}
}

static void update_bot_right(){
	printf("Ditermining bot left setting");
	if (strcmp(bottemRight, "WeatherUpdateTime") == 0){
		printf("bot_left setting to: WeatherUpdateTime");
		text_layer_set_text(s_bottem_right_layer, calculate_data_time_difference());
	}
	else if (strcmp(bottemRight, "CaloriesBurned") == 0){
		printf("bot_left setting to: CaloriesBurned");
		text_layer_set_text(s_bottem_right_layer, calorie_count_str);
		//layer_set_update_proc(text_layer_get_layer(s_bottem_left_layer), calories_burned_update_proc);
	}
	else if (strcmp(bottemRight, "TimeSlept") == 0){
		printf("bot_left setting to: TimeSlept");
		text_layer_set_text(s_bottem_right_layer, sleep_time_string);
		//layer_set_update_proc(text_layer_get_layer(s_bottem_left_layer), time_slept_update_proc);
	}
	else if (strcmp(bottemRight, "BatteryChargeTime") == 0){
		printf("bot_left setting to: BatteryChargeTime");
		text_layer_set_text(s_bottem_right_layer, calculate_battery_time_difference());
		
		//layer_set_update_proc(text_layer_get_layer(s_bottem_left_layer), battery_charge_time_update_proc);
	}
	else{
		printf("Invalid bottem right Identifier");
		printf("Recieved: %s", bottemRight);
	}
}

static void updateDisplay(){
	update_bot_left();
	update_bot_right();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if (accelBuffer > 0){
		accelBuffer--;
	}
	size_t memoryUsage = heap_bytes_used();
	size_t memoryFree = heap_bytes_free();
	printf("Memory currently used is: %zu/%zu", memoryUsage, memoryFree + memoryUsage);
	if (!asleep){
		printf("Seconds are: %d", tick_time->tm_sec);
		if (tick_time->tm_sec == 59 || second_counter > 0){
			if (second_counter > 0){
				int testTime = tick_time->tm_sec;
				static char batteryBuffer[10];
				snprintf(batteryBuffer, sizeof(batteryBuffer), "%d", testTime);
				printf("BatteryBuffer is: %s", batteryBuffer);
				text_layer_set_text(s_seconds_layer, batteryBuffer);
			}
			if (second_counter == num_seconds){
				layer_set_hidden(text_layer_get_layer(s_seconds_layer), false);
				layer_set_hidden(text_layer_get_layer(s_battery_charge_layer), true);
			}
			if (second_counter == 1){
				layer_set_hidden(text_layer_get_layer(s_seconds_layer), true);
				layer_set_hidden(text_layer_get_layer(s_battery_charge_layer), false);
			}
			if (second_counter > 0){
				second_counter--;
			}
            size_t memoryUsageStart = heap_bytes_used();
            size_t memoryUsageFun = heap_bytes_used();
            size_t memoryUsageNow;
			update_time();          
			update_step_average();
			calculate_data_time_difference();
			calculate_battery_time_difference();
			updateWeather();
            size_t memoryUsageEnd = heap_bytes_used();
			updateDisplay();
            printf("used memory at the end: %d", memoryUsageEnd);
		}

	}
	else{
		printf("I am asleep");
		if (window_stack_get_top_window() == s_main_window){
			window_stack_push(s_sleep_window, true);
		}
		if (asleep_time > 0){
			asleep_time --;
		}
		else{
			asleep = false;
		}
	}
}

static void main_window_load(Window *window) {
  // Get information about the Window
	BatteryChargeState batteryState = battery_state_service_peek();

	Layer *window_layer = window_get_root_layer(window);

	window_set_background_color(window, GColorBlack);

	GRect bounds = layer_get_bounds(window_layer);

	s_bgcolor_layer = text_layer_create(bounds);
	text_layer_set_background_color(s_bgcolor_layer, bg_color);
	layer_add_child(window_layer,text_layer_get_layer(s_bgcolor_layer));

	printf("%"PRIu32 , persist_read_int(KEY_TEMPERATURE));
	for (int i = 0; i < TEMP_DATA_POINTS; i++){
		tempData[i] = persist_read_int(i + KEY_TEMPERATURE);
	}

	for (int i = 0; i < POP_DATA_POINTS; i++){
		popData[i] = persist_read_int(i + KEY_POPDATA);
	}
	curent_humidity = persist_read_int(KEY_HUMIDITY);

	printf("Curent Humidity Cached: %" SCNd32 "\n", persist_read_int(KEY_HUMIDITY));

	get_step_average();
	get_step_count();
	get_step_goal();

	// Create GFont
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIGITAL_FONT_48));
	s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
	s_battery_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
	s_weather_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);

	if(bat_behind_graph){
		s_battery_behind_clock = layer_create(GRect(0, 0, 110, 40));
		layer_set_update_proc(s_battery_behind_clock, s_battery_behind_clock_update_proc);
		layer_add_child(window_layer,s_battery_behind_clock);
	}

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
	text_layer_set_text_color(s_time_layer, time_color);
	text_layer_set_text(s_time_layer, "00:00");
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Apply to TextLayer
	text_layer_set_font(s_time_layer, s_time_font);



  // Create temperature Layer
	s_weather_layer = text_layer_create(
		GRect(0, 126, bounds.size.w, 30));


  // Style the text
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_text_color(s_weather_layer, weather_color);
	text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
	static char weather_layer_buffer[100];
	persist_read_string(KEY_WEATHER_STRING, weather_layer_buffer, 100);
	printf("Character buffer0 is %s", weather_layer_buffer);

  // Create second custom font, apply it and add to Window
	s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
	text_layer_set_font(s_weather_layer, s_date_font);
	text_layer_set_text_color(s_weather_layer, weather_color);
	text_layer_set_text(s_weather_layer, weather_layer_buffer);
	layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));

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
	layer_set_update_proc(bitmap_layer_get_layer(s_bluetooth_image), bluetooth_off_proc);

	s_bluetooth_on = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ON);
	s_bluetooth_image_on = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_bluetooth_image_on, s_bluetooth_on);
	layer_set_frame(bitmap_layer_get_layer(s_bluetooth_image_on), GRect(123, -1, 26, 26));
	layer_add_child(window_layer,bitmap_layer_get_layer(s_bluetooth_image_on));
	layer_set_update_proc(bitmap_layer_get_layer(s_bluetooth_image_on), bluetooth_on_proc);

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
	text_layer_set_text_color(s_battery_charge_layer, battery_percentage_color);
	text_layer_set_text_alignment(s_battery_charge_layer, GTextAlignmentCenter);
	layer_add_child(window_layer,text_layer_get_layer(s_battery_charge_layer));

	s_seconds_layer = text_layer_create(GRect(113, 24, 30, 40));
	text_layer_set_font(s_seconds_layer, s_battery_font);
	text_layer_set_background_color(s_seconds_layer, GColorClear);
	text_layer_set_text_color(s_seconds_layer, battery_percentage_color);
	text_layer_set_text_alignment(s_seconds_layer, GTextAlignmentCenter);
	layer_set_hidden(text_layer_get_layer(s_seconds_layer), true);
	layer_add_child(window_layer,text_layer_get_layer(s_seconds_layer));


	s_date_layer = text_layer_create(GRect(0, 34, 144, 25));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, date_color);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	text_layer_set_font(s_date_layer, s_date_font);
	layer_add_child(window_layer,text_layer_get_layer(s_date_layer));

	s_steps_layer = text_layer_create(GRect(20, 105, 40, 25));
	text_layer_set_background_color(s_steps_layer, GColorClear);
	text_layer_set_text_color(s_steps_layer, steps_color);
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
	layer_set_update_proc(bitmap_layer_get_layer(s_steps_above_layer), steps_above_update_proc);

	s_steps_below_image = gbitmap_create_with_resource(RESOURCE_ID_STEPS_BELOW);
	s_steps_below_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_steps_below_layer, s_steps_below_image);
	layer_set_frame(bitmap_layer_get_layer(s_steps_below_layer), GRect(0, 111, 20, 20));
	layer_add_child(window_layer,bitmap_layer_get_layer(s_steps_below_layer));
	layer_set_update_proc(bitmap_layer_get_layer(s_steps_below_layer), steps_below_update_proc);
	

	update_step_average();

	s_humidity_layer = text_layer_create(GRect(95, 104, 40, 25));
	text_layer_set_background_color(s_humidity_layer, GColorClear);
	text_layer_set_text_alignment(s_humidity_layer, GTextAlignmentLeft);
	text_layer_set_font(s_humidity_layer, s_date_font);
	text_layer_set_text_color(s_humidity_layer, humidity_color);
	layer_add_child(window_layer,text_layer_get_layer(s_humidity_layer));
	text_layer_set_overflow_mode(s_humidity_layer, GTextOverflowModeWordWrap);
	update_humidity();


	s_bottem_left_layer = text_layer_create(GRect(0, 153, 50, 25));
	text_layer_set_background_color(s_bottem_left_layer, GColorClear);
	text_layer_set_text_color(s_bottem_left_layer, data_time_color);
	text_layer_set_text_alignment(s_bottem_left_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(s_bottem_left_layer, GTextOverflowModeWordWrap);
	text_layer_set_font(s_bottem_left_layer, s_battery_font);
	text_layer_set_text_color(s_bottem_left_layer, data_time_color);
	calculate_data_time_difference();
	
	layer_mark_dirty(text_layer_get_layer(s_bottem_left_layer));
	layer_add_child(window_layer,text_layer_get_layer(s_bottem_left_layer));
	update_calorie_count();
	update_sleep_time();


	s_bottem_right_layer = text_layer_create(GRect(94, 153, 50, 25));
	text_layer_set_background_color(s_bottem_right_layer, GColorClear);
	text_layer_set_text_color(s_bottem_right_layer, battery_life_color);
	text_layer_set_text_alignment(s_bottem_right_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(s_bottem_right_layer, GTextOverflowModeWordWrap);
	text_layer_set_font(s_bottem_right_layer, s_battery_font);
	layer_add_child(window_layer,text_layer_get_layer(s_bottem_right_layer));

	calculate_battery_time_difference();
    
    s_wind_speed_layer = text_layer_create(GRect(0, 153, 144, 25));
    text_layer_set_text_color(s_wind_speed_layer, GColorWhite);
    text_layer_set_background_color(s_wind_speed_layer, GColorClear);
	text_layer_set_text_alignment(s_wind_speed_layer, GTextAlignmentCenter);
	text_layer_set_font(s_wind_speed_layer, s_battery_font);
    layer_add_child(window_layer,text_layer_get_layer(s_wind_speed_layer));
    updateWindSpeed();
    

	time_t storedTime;
	unsigned long testLong;
	persist_read_data(KEY_TIME, &storedTime, sizeof(storedTime));
	testLong = (unsigned long)storedTime;
	printf("Time currently in storage (from loading main window) is : %lu", (unsigned long)testLong);

	if(!persist_exists(KEY_BATTERY_TIME)){
		time_t batteryTime = time(NULL);
		persist_write_data(KEY_BATTERY_TIME, &batteryTime, sizeof(batteryTime));
	}

	s_lightningbolt = gbitmap_create_with_resource(RESOURCE_ID_LIGHTNINGBOLT);
	s_battery_charging_layer = bitmap_layer_create(GRect(112, 7, 26, 26));
	bitmap_layer_set_bitmap(s_battery_charging_layer, s_lightningbolt);
	layer_set_update_proc(bitmap_layer_get_layer(s_battery_charging_layer), lightningbolt_update_proc);
	if (batteryState.is_charging){
		layer_set_hidden(bitmap_layer_get_layer(s_battery_charging_layer), false);
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(s_battery_charging_layer), true);
	}
	layer_add_child(window_layer,bitmap_layer_get_layer(s_battery_charging_layer));







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

	accel_tap_service_subscribe(accel_tap_handler);
	battery_state_service_subscribe(battery_handler);
  // Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	
	updateDisplay();
	
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

static void sleep_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	s_sleep_layer = text_layer_create(bounds);
	text_layer_set_background_color(s_sleep_layer, GColorClear);
	text_layer_set_text_color(s_sleep_layer, GColorWhite);
	text_layer_set_text_alignment(s_sleep_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(s_sleep_layer, GTextOverflowModeWordWrap);
	text_layer_set_font(s_sleep_layer, s_date_font);
	text_layer_set_text(s_sleep_layer, "The watchface has entered sleep mode.  Shake to wake it up." );
	layer_add_child(window_layer,text_layer_get_layer(s_sleep_layer));
}

static void sleep_window_unload(Window *window){
	text_layer_destroy(s_sleep_layer);
}



void prv_init(void) {
  // ...

  // Open AppMessage connection
  //app_message_register_inbox_received(prv_inbox_received_handler);

  // ...
}

static void checkStorage(){
	//todo Generlize this funciton.
	int numKeys = 0;
	GColor batteryunchargedcolor;
	GColor defaultColor;
	bool defaultBool;
	if(!persist_exists(KEY_TEMPERATURE)){
		for (int i = 0; i < TEMP_DATA_POINTS; i++){
			persist_write_int(KEY_TEMPERATURE + i, 0);
			numKeys++;
		}
	}
	if(!persist_exists(KEY_POPDATA)){
		for (int i = 0; i < POP_DATA_POINTS; i++){
			persist_write_int(KEY_POPDATA + i, 0);
			numKeys++;
		}
	}
	if(!persist_exists(KEY_HUMIDITY)){
		persist_write_int(KEY_HUMIDITY, 0);
		numKeys++;
	}
	if(!persist_exists(KEY_TIME)){
		time_t curentTime = time(NULL);
		persist_write_data(KEY_TIME, &curentTime, sizeof(curentTime));
		numKeys++;
	}
	if(!persist_exists(KEY_WEATHER_STRING)){
		persist_write_string(KEY_WEATHER_STRING, "Loading...");
		numKeys++;
	}
	if(!persist_exists(KEY_BATTERY_TIME)){
		time_t theTime = time(NULL);
		persist_write_data(KEY_BATTERY_TIME, &theTime, sizeof(theTime));
		numKeys++;
	}
	if(!persist_exists(KEY_BACKGROUNDCOLOR)){
		GColor bgcolor = GColorBlack;
		persist_write_data(KEY_BACKGROUNDCOLOR, &bgcolor, sizeof(bgcolor));
		numKeys++;
	}
	if(!persist_exists(KEY_GRID_COLOR)){
		GColor gridcolor = GColorGreen;
		persist_write_data(KEY_GRID_COLOR, &gridcolor, sizeof(gridcolor));
		numKeys++;
	}
	if(!persist_exists(KEY_BATTERY_COLOR)){
		GColor batterycolor = GColorIslamicGreen;
		persist_write_data(KEY_BATTERY_COLOR, &batterycolor, sizeof(batterycolor));
		numKeys++;
	}
	if(!persist_exists(KEY_TEMP_COLOR)){
		GColor tempcolor = GColorRed;
		persist_write_data(KEY_TEMP_COLOR, &tempcolor, sizeof(tempcolor));
		numKeys++;
	}
	if(!persist_exists(KEY_POP_COLOR)){
		GColor popcolor = GColorBlue;
		persist_write_data(KEY_POP_COLOR, &popcolor, sizeof(popcolor));
		numKeys++;
	}
	if(!persist_exists(KEY_BATTERY_UNCHARGED)){
		GColor batteryunchargedcolor = GColorDarkCandyAppleRed;
		persist_write_data(KEY_BATTERY_UNCHARGED, &batteryunchargedcolor, sizeof(batteryunchargedcolor));
		numKeys++;
	} 
	if(!persist_exists(KEY_TIME_COLOR)){
		GColor timecolor = GColorWhite;
		persist_write_data(KEY_TIME_COLOR, &timecolor, sizeof(timecolor));
		numKeys++;
	} 
	if(!persist_exists(KEY_DATE_COLOR)){
		batteryunchargedcolor = GColorWhite;
		persist_write_data(KEY_DATE_COLOR, &batteryunchargedcolor, sizeof(batteryunchargedcolor));
		numKeys++;
	} 
	if(!persist_exists(KEY_BATTERY_PERCENTAGE)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_BATTERY_PERCENTAGE, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_TEMP_NUMBER_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_TEMP_NUMBER_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_STEPS_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_STEPS_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_DATA_TIME_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_DATA_TIME_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_BATTERY_LIFE_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_BATTERY_LIFE_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_WEATHER_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_WEATHER_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_HUMIDITY_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_HUMIDITY_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_GRAPH_LINE_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_GRAPH_LINE_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 
	if(!persist_exists(KEY_BATTERY_OUTLINE_COLOR)){
		defaultColor = GColorWhite;
		persist_write_data(KEY_BATTERY_OUTLINE_COLOR, &defaultColor, sizeof(defaultColor));
		numKeys++;
	} 

	if(!persist_exists(KEY_BATTERY_BEHIND_CLOCK_TOGGLE)){
		defaultBool = true;
		persist_write_data(KEY_BATTERY_BEHIND_CLOCK_TOGGLE, &defaultBool, sizeof(defaultBool));
		numKeys++;
	} 
	if(!persist_exists(KEY_SECONDS_TICK)){
		defaultBool = true;
		persist_write_data(KEY_SECONDS_TICK, &defaultBool, sizeof(defaultBool));
		numKeys++;
	} 
	if(!persist_exists(KEY_SECONDS_COUNT)){
		int secondsSlider = 10;
		persist_write_data(KEY_SECONDS_COUNT, &secondsSlider, sizeof(secondsSlider));
		numKeys++;
	} 
	if(!persist_exists(KEY_SLEEP_MODE_TOGGLE)){
		defaultBool = true;
		persist_write_data(KEY_SLEEP_MODE_TOGGLE, &defaultBool, sizeof(defaultBool));
		numKeys++;
	}
	if(!persist_exists(KEY_GRID_COLOR_CHANGE_TOGGLE)){
		defaultBool = true;
		persist_write_data(KEY_GRID_COLOR_CHANGE_TOGGLE, &defaultBool, sizeof(defaultBool));
		numKeys++;
	} 
	if(!persist_exists(KEY_TRIPLE_SHAKE)){
		defaultBool = true;
		persist_write_data(KEY_TRIPLE_SHAKE, &defaultBool, sizeof(defaultBool));
		numKeys++;
	}
	if(!persist_exists(KEY_CELCIUS_TOGGLE)){
		defaultBool = false;
		persist_write_data(KEY_CELCIUS_TOGGLE, &defaultBool, sizeof(defaultBool));
		numKeys++;
	}
	
	//Strings
	//DEFINATLY NEEDS TO BE FIXED.  persist_write_data vs persist_write-string.
	//TODO
	if(!persist_exists(KEY_BOTTEM_LEFT)){
		char bottem_left[100] = "WeatherUpdateTime";
		persist_write_data(KEY_BOTTEM_LEFT, &bottem_left, sizeof(bottem_left));
		numKeys++;
	}
	if(!persist_exists(KEY_BOTTEM_RIGHT)){
		char bottem_right[100] = "BatteryChargeTime";
		persist_write_data(KEY_BOTTEM_RIGHT, &bottem_right, sizeof(bottem_right));
		numKeys++;
	}
	forceWeatherUpdate();
	
	printf("Generated %d defaults in storage", numKeys);
	storeOptions();
}

static void getAppMessageResult(AppMessageResult reason, void *context){

	switch(reason){
		case APP_MSG_OK:
		snprintf(reasonStr,20,"%s","APP_MSG_OK");
		break;
		case APP_MSG_SEND_TIMEOUT:
		snprintf(reasonStr,20,"%s","SEND TIMEOUT");
		break;
		case APP_MSG_SEND_REJECTED:
		snprintf(reasonStr,20,"%s","SEND REJECTED");
		break;
		case APP_MSG_NOT_CONNECTED:
		snprintf(reasonStr,20,"%s","NOT CONNECTED");
		break;
		case APP_MSG_APP_NOT_RUNNING:
		snprintf(reasonStr,20,"%s","NOT RUNNING");
		break;
		case APP_MSG_INVALID_ARGS:
		snprintf(reasonStr,20,"%s","INVALID ARGS");
		break;
		case APP_MSG_BUSY:
		snprintf(reasonStr,20,"%s","BUSY");
		break;
		case APP_MSG_BUFFER_OVERFLOW:
		snprintf(reasonStr,20,"%s","BUFFER OVERFLOW");
		break;
		case APP_MSG_ALREADY_RELEASED:
		snprintf(reasonStr,20,"%s","ALRDY RELEASED");
		break;
		case APP_MSG_CALLBACK_ALREADY_REGISTERED:
		snprintf(reasonStr,20,"%s","CLB ALR REG");
		break;
		case APP_MSG_CALLBACK_NOT_REGISTERED:
		snprintf(reasonStr,20,"%s","CLB NOT REG");
		break;
		case APP_MSG_OUT_OF_MEMORY:
		snprintf(reasonStr,20,"%s","OUT OF MEM");
		break;
		case APP_MSG_CLOSED:
		snprintf(reasonStr,20,"%s","CLOSED APP MSG");
		break;
		case APP_MSG_INTERNAL_ERROR:
		snprintf(reasonStr,20,"%s","INTERNAL ERROR");
		break;
		case APP_MSG_INVALID_STATE:
		snprintf(reasonStr,20,"%s","INVALID STATE");
		break;
	}
	printf("App Message fail because of: %s", reasonStr);
}
static void reloadMainWindow(){
    //window_stack_remove(s_main_window, false);
   // s_main_window = window_create();
   // window_set_background_color(s_main_window, GColorBlack);
   // window_stack_push(s_main_window, false);
    // Set handlers to manage the elements inside the Window
	//window_set_window_handlers(s_main_window, (WindowHandlers) {
	//	.load = main_window_load,
	//	.unload = main_window_unload
	//});
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read color preferences
	printf("Recieved configuration settngs");
	Tuple *bg_color_t = dict_find(iter, KEY_BACKGROUNDCOLOR);
	if(bg_color_t) {
		printf("BG color is: %" PRId32, bg_color_t->value->int32);
		bg_color = GColorFromHEX(bg_color_t->value->int32);
		text_layer_set_background_color(s_bgcolor_layer, bg_color);
		persist_write_data(KEY_BACKGROUNDCOLOR, &bg_color, sizeof(bg_color));
	}

	Tuple *grid_color_t = dict_find(iter, KEY_GRID_COLOR);
	if(grid_color_t) {
		grid_color = GColorFromHEX(grid_color_t->value->int32);
		persist_write_data(KEY_GRID_COLOR, &grid_color, sizeof(grid_color));
	}
	Tuple *battery_color_t = dict_find(iter, KEY_BATTERY_COLOR);
	if(battery_color_t) {
		battery_color = GColorFromHEX(battery_color_t->value->int32);
		persist_write_data(KEY_BATTERY_COLOR, &battery_color, sizeof(battery_color));
	}
	Tuple *temp_color_t = dict_find(iter, KEY_TEMP_COLOR);
	if(temp_color_t) {
		temp_color = GColorFromHEX(temp_color_t->value->int32);
		persist_write_data(KEY_TEMP_COLOR, &temp_color, sizeof(temp_color));
	}
	Tuple *pop_color_t = dict_find(iter, KEY_POP_COLOR);
	if(pop_color_t) {
		pop_color = GColorFromHEX(pop_color_t->value->int32);
		persist_write_data(KEY_POP_COLOR, &pop_color, sizeof(pop_color));
	}
	Tuple *uncharged_color_t = dict_find(iter, KEY_BATTERY_UNCHARGED);
	if(uncharged_color_t) {
		uncharged_color = GColorFromHEX(uncharged_color_t->value->int32);
		persist_write_data(KEY_BATTERY_UNCHARGED, &uncharged_color, sizeof(uncharged_color));
	}
	Tuple *time_color_t = dict_find(iter, KEY_TIME_COLOR);
	if(time_color_t) {
		time_color = GColorFromHEX(time_color_t->value->int32);
		persist_write_data(KEY_TIME_COLOR, &time_color, sizeof(time_color));
	}
	Tuple *date_color_t = dict_find(iter, KEY_DATE_COLOR);
	if(date_color_t) {
		date_color = GColorFromHEX(date_color_t->value->int32);
		persist_write_data(KEY_DATE_COLOR, &date_color, sizeof(date_color));
	}
	Tuple *battery_percentage_color_t = dict_find(iter, KEY_BATTERY_PERCENTAGE);
	if(battery_percentage_color_t) {
		battery_percentage_color = GColorFromHEX(battery_percentage_color_t->value->int32);
		persist_write_data(KEY_BATTERY_PERCENTAGE, &battery_percentage_color, sizeof(battery_percentage_color));
	}
	Tuple *temp_number_color_t = dict_find(iter, KEY_TEMP_NUMBER_COLOR);
	if(temp_number_color_t) {
		temp_number_color = GColorFromHEX(temp_number_color_t->value->int32);
		persist_write_data(KEY_TEMP_NUMBER_COLOR, &temp_number_color, sizeof(temp_number_color));
	}
	Tuple *steps_color_t = dict_find(iter, KEY_STEPS_COLOR);
	if(steps_color_t) {
		steps_color = GColorFromHEX(steps_color_t->value->int32);
		persist_write_data(KEY_STEPS_COLOR, &steps_color, sizeof(steps_color));
	}
	Tuple *data_time_color_t = dict_find(iter, KEY_DATA_TIME_COLOR);
	if(data_time_color_t) {
		data_time_color = GColorFromHEX(data_time_color_t->value->int32);
		persist_write_data(KEY_DATA_TIME_COLOR, &data_time_color, sizeof(data_time_color));
	}
	Tuple *battery_life_color_t = dict_find(iter, KEY_BATTERY_LIFE_COLOR);
	if(battery_life_color_t) {
		battery_life_color = GColorFromHEX(battery_life_color_t->value->int32);
		persist_write_data(KEY_BATTERY_LIFE_COLOR, &battery_life_color, sizeof(battery_life_color));
	}
	Tuple *weather_color_t = dict_find(iter, KEY_WEATHER_COLOR);
	if(weather_color_t) {
		weather_color = GColorFromHEX(weather_color_t->value->int32);
		persist_write_data(KEY_WEATHER_COLOR, &weather_color, sizeof(weather_color));
	}
	Tuple *humidity_color_t = dict_find(iter, KEY_HUMIDITY_COLOR);
	if(humidity_color_t) {
		humidity_color = GColorFromHEX(humidity_color_t->value->int32);
		persist_write_data(KEY_HUMIDITY_COLOR, &humidity_color, sizeof(humidity_color));
	}
	Tuple *graph_line_color_t = dict_find(iter, KEY_GRAPH_LINE_COLOR);
	if(graph_line_color_t) {
		graph_line_color = GColorFromHEX(graph_line_color_t->value->int32);
		persist_write_data(KEY_GRAPH_LINE_COLOR, &graph_line_color, sizeof(graph_line_color));
	}
	Tuple *battery_outline_color_t = dict_find(iter, KEY_BATTERY_OUTLINE_COLOR);
	if(battery_outline_color_t) {
		battery_outline_color = GColorFromHEX(battery_outline_color_t->value->int32);
		persist_write_data(KEY_BATTERY_OUTLINE_COLOR, &battery_outline_color, sizeof(battery_outline_color));
	}

  // Read boolean preferences
	Tuple *second_tick_t = dict_find(iter, KEY_SECONDS_TICK);
	if(second_tick_t) {
		show_seconds = second_tick_t->value->int32 == 1;
		persist_write_data(KEY_SECONDS_COUNT, &show_seconds, sizeof(show_seconds));
	}
	Tuple *bat_behind_graph_t = dict_find(iter, KEY_BATTERY_BEHIND_CLOCK_TOGGLE);
	if(bat_behind_graph_t) {
		bat_behind_graph = bat_behind_graph_t->value->int32 == 1;
		persist_write_data(KEY_BATTERY_BEHIND_CLOCK_TOGGLE, &bat_behind_graph, sizeof(bat_behind_graph));
	}
	Tuple *sleep_mode_toggle_t = dict_find(iter, KEY_SLEEP_MODE_TOGGLE);
	if(sleep_mode_toggle_t) {
		bool sleep_mode_toggle = sleep_mode_toggle_t->value->int32 == 1;
		persist_write_data(KEY_SLEEP_MODE_TOGGLE, &sleep_mode_toggle, sizeof(sleep_mode_toggle));
	}
	Tuple *grid_color_toggle_t = dict_find(iter, KEY_GRID_COLOR_CHANGE_TOGGLE);
	if(grid_color_toggle_t) {
		bool grid_color_toggle = grid_color_toggle_t->value->int32 == 1;
		persist_write_data(KEY_GRID_COLOR_CHANGE_TOGGLE, &grid_color_toggle, sizeof(grid_color_toggle));
	}
	Tuple *triple_shake_set_t = dict_find(iter, KEY_TRIPLE_SHAKE);
	if(triple_shake_set_t) {
		bool triple_shake_set = triple_shake_set_t->value->int32 == 1;
		persist_write_data(KEY_TRIPLE_SHAKE, &triple_shake_set, sizeof(triple_shake_set));
	}
	Tuple *celcius_set_t = dict_find(iter, KEY_CELCIUS_TOGGLE);
	if(celcius_set_t) {
		bool celcius_set = celcius_set_t->value->int32 == 1;
		persist_write_data(KEY_CELCIUS_TOGGLE, &celcius_set, sizeof(celcius_set));
	}
        //read slider preferences
	Tuple *seconds_count_t = dict_find(iter, KEY_SECONDS_COUNT);
	if(seconds_count_t) {
		num_seconds = seconds_count_t->value->int32;
		persist_write_data(KEY_SECONDS_COUNT, &num_seconds, sizeof(num_seconds));
	}
	
	//read string preferences
	Tuple *bottem_left_t = dict_find(iter, KEY_BOTTEM_LEFT);
	if(bottem_left_t) {
		char *bottem_left = bottem_left_t->value->cstring;
		printf("Got string: %s", bottem_left);
		persist_write_string(KEY_BOTTEM_LEFT, bottem_left);
		char retrieveTest[30];
		persist_read_string(KEY_BOTTEM_LEFT, retrieveTest, sizeof(retrieveTest));
	}
	Tuple *bottem_right_t = dict_find(iter, KEY_BOTTEM_RIGHT);
	if(bottem_right_t) {
		char *bottem_right = bottem_right_t->value->cstring;
		printf("Got string: %s", bottem_right);
		persist_write_string(KEY_BOTTEM_RIGHT, bottem_right);
	}


	Tuple *animations_t = dict_find(iter, KEY_ANIMATIONS);
	if(animations_t) {
		bool animations = animations_t->value->int32 == 1;
	}
	if (window_stack_contains_window(s_main_window)){
		reloadMainWindow();
	}

	
	storeOptions();


}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	Tuple *ready_tuple = dict_find(iterator, MESSAGE_KEY_JSReady);
	if(ready_tuple) {
		printf("Pebble JS is ready in C");
		// PebbleKit JS is ready! Safe to send messages
		s_js_ready = true;
	}
	if (dict_find(iterator, KEY_HUMIDITY) == NULL){
		printf("Recieved configuration Data");
		prv_inbox_received_handler(iterator, context);
	}
	else{
		printf("Recieved Weather Data");
		printf("Starting to Parse Weather Data");
        
        Tuple *wind_direction_t = dict_find(iterator, KEY_CURENT_WIND_DIRECTION);
        printf("hi1");
		int windDirection = wind_direction_t->value->int32;
		printf("WInd direction in C is: %d", windDirection);
		persist_write_int(KEY_CURENT_WIND_DIRECTION, windDirection);
        printf("Parsing wind direction");
		Tuple *wind_speed_t = dict_find(iterator, KEY_WIND_SPEED_CURENT);
		persist_write_int(KEY_WIND_SPEED_CURENT, wind_speed_t->value->int32);
		updateWindSpeed();

	//persist_write_int(103, time(NULL));
		time_t curentTime = time(NULL);
		printf("Storing Recieved Time: %" SCNd32 "\n", time(NULL));
		persist_write_data(KEY_TIME, &curentTime, sizeof(curentTime));
		dataTime = curentTime;

		time_t storedTime;
		unsigned long testLong;
		persist_read_data(KEY_TIME, &storedTime, sizeof(storedTime));
		testLong = storedTime;
		printf("unsigned long currently in storage is unsigned long: %lu", testLong);

  // Store incoming information
		static char temperature_buffer[8];
		static char conditions_buffer[32];
		static char weather_layer_buffer[32];
		static char humidity_buffer[10];
		curent_humidity = (int)dict_find(iterator, KEY_HUMIDITY)->value->int32;
		snprintf(humidity_buffer, sizeof(humidity_buffer), "%d%%", curent_humidity);
		text_layer_set_text_color(s_humidity_layer, humidity_color);
		text_layer_set_text(s_humidity_layer, humidity_buffer);
		persist_write_int(KEY_HUMIDITY, curent_humidity);

		Tuple* temps [TEMP_DATA_POINTS];
		Tuple* conds [1];
		Tuple* pops [POP_DATA_POINTS];
		char conditionStrings[32];
		conditionStrings[0] = conditionStrings[0];
		int tempInts[TEMP_DATA_POINTS + 1];
		for (int i = 0; i < TEMP_DATA_POINTS + 1; i++){
			tempInts[i] = i;
		}
		for (int i = 0; i < TEMP_DATA_POINTS; i++){
			temps[i] = dict_find(iterator, i + KEY_TEMPERATURE);
		}
		for (int i = 0; i < POP_DATA_POINTS; i++){
			pops[i] = dict_find(iterator, i + KEY_POPDATA);
		}
		conds[0] = dict_find(iterator, KEY_WEATHER_STRING);
		printf("Celcius is: %s", celcius ? "true" : "false");
		if(celcius){
			snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temps[0]->value->int32);
		}
		else{
			snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)temps[0]->value->int32);
		}
		for (int i = 0; i < TEMP_DATA_POINTS; i++){
			tempInts[i] = (int)temps[i]->value->int32;
			tempData[i] = tempInts[i];
			persist_write_int(i + KEY_TEMPERATURE, tempInts[i]);
		//int test = c.value;   
		}
		for (int i = 0; i < POP_DATA_POINTS; i++){
			popData[i] = (int)pops[i]->value->int32;
			persist_write_int(i + KEY_POPDATA, popData[i]);
		//int test = c.value;    
		}

		snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conds[0]->value->cstring);
		conditionStrings[0] = conditions_buffer[0];
		printf("Condition Buffer: %s", conditions_buffer);

		static char tempDisplayBuffer[8];
		static char condDisplayBuffer[32];
		if(celcius){
			snprintf(tempDisplayBuffer, sizeof(tempDisplayBuffer), "%dC", (int)temps[0]->value->int32);
		}
		else{
			snprintf(tempDisplayBuffer, sizeof(tempDisplayBuffer), "%dF", (int)temps[0]->value->int32);
		}
		snprintf(condDisplayBuffer, sizeof(condDisplayBuffer), "%s", conds[0]->value->cstring);
		snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", tempDisplayBuffer, condDisplayBuffer);
		text_layer_set_text_color(s_weather_layer, weather_color);
		text_layer_set_text(s_weather_layer, weather_layer_buffer);
		persist_write_string(KEY_WEATHER_STRING, weather_layer_buffer);
		printf("%s", "Finished Parsing Weather Data");
		layer_mark_dirty(s_graph_background);
		calculate_data_time_difference();
	}

}



static void init() {
  // Create main Window element and assign to pointer
	checkStorage();

	s_main_window = window_create();
	prv_init();

  // Set the background color
	window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	s_sleep_window = window_create();

  // Set the background color
	window_set_background_color(s_sleep_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_sleep_window, (WindowHandlers) {
		.load = sleep_window_load,
		.unload = sleep_window_unload
	});

  // Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
	update_time();

  // Register with TickTimerService
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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
	app_message_register_inbox_dropped(getAppMessageResult);
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