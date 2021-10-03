#include "displays/ili9486.h"
#include "gui.h"
#include "hid.h"
#include "millis.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include <sstream>
#include <string>

display::Ili9486 *global_display;
Ui *global_ui;
static unsigned short blink_interval_ms = UsbState::not_mounted;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {

	if (area->x2 < 0 || area->y2 < 0 || area->x1 > (disp->hor_res - 1) || area->y1 > (disp->ver_res - 1)) {
		lv_disp_flush_ready(disp);
		return;
	}

	/* Truncate the area to the screen */
	int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
	int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
	int32_t act_x2 = area->x2 > disp->hor_res - 1 ? disp->hor_res - 1 : area->x2;
	int32_t act_y2 = area->y2 > disp->ver_res - 1 ? disp->ver_res - 1 : area->y2;

	global_display->command(display::CASET);
	global_display->data(act_x1 >> 8);
	global_display->data(act_x1 >> 0);
	global_display->data(act_x2 >> 8);
	global_display->data(act_x2 >> 0);

	global_display->command(display::RASET);
	global_display->data(act_y1 >> 8);
	global_display->data(act_y1 >> 0);
	global_display->data(act_y2 >> 8);
	global_display->data(act_y2 >> 0);

	global_display->command(display::RAMWR);

    uint16_t *ds = (uint16_t *)color_p;

	for (int32_t y{act_y1}; y <= act_y2; y++) {
		for (int32_t x{act_x1}; x <= act_x2; x++) {
			global_display->data16(*ds);
			ds++;
		}
	}
    
	lv_disp_flush_ready(disp);
}

void led_blinking_task(void) {
	static unsigned long start_ms = 0;
	static bool led_state = false;

	// Blink every interval ms
	if (millis() - start_ms < blink_interval_ms)
		return; // not enough time
	start_ms += blink_interval_ms;

	gpio_put(PICO_DEFAULT_LED_PIN, led_state);
	led_state = 1 - led_state; // toggle
}

void core1_entry() {
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	tusb_init();

	while (true) {
		tud_task();
		led_blinking_task();
	}
}

int main() {
	lv_init();
	//*spi, cs, dc, mosi, sclk, rst , bl
	// spi0, 17, 20, 19,     18, 21, 0
	display::Ili9486 tft(spi0, 17, 20, 19, 18, 21);
	global_display = &tft;
	tft.rotation(90);
	unsigned short hor_res{};
	unsigned short ver_res{};
	tft.get_display_size(hor_res, ver_res);


	static lv_disp_draw_buf_t draw_buf;
	static lv_color_t *buf1{new lv_color_t[hor_res * ver_res / 10]};
	static lv_color_t *buf2{new lv_color_t[hor_res * ver_res / 10]};
	lv_disp_draw_buf_init(&draw_buf, buf1, buf2, hor_res * ver_res / 10);

	static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.draw_buf = &draw_buf;
	disp_drv.hor_res = hor_res;
	disp_drv.ver_res = ver_res;
	lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

	Ui test(disp);
	global_ui = &test;
	global_ui->show_splash_screen(true);

	multicore_launch_core1(core1_entry);

	while (true) {
		lv_task_handler();
		lv_timer_handler();
		sleep_ms(5);
		if (blink_interval_ms == UsbState::mounted) {
			global_ui->show_splash_screen(false);
		}
		else {
			global_ui->show_splash_screen(true);
		}
	}
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) { blink_interval_ms = UsbState::mounted; }

// Invoked when device is unmounted
void tud_umount_cb(void) { blink_interval_ms = UsbState::not_mounted; }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
	(void)remote_wakeup_en;
	blink_interval_ms = UsbState::suspended;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) { 
	blink_interval_ms = UsbState::mounted; 
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
							   hid_report_type_t report_type, uint8_t *buffer,
							   uint16_t reqlen) {
	// TODO not Implemented
	(void)itf;
	(void)report_id;
	(void)report_type;
	(void)buffer;
	(void)reqlen;

	return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
						   hid_report_type_t report_type, uint8_t const *buffer,
						   uint16_t bufsize) {
	(void)itf;
	(void)report_id;
	(void)report_type;

	std::string fdsf(buffer, buffer + bufsize);
	std::string line;
	std::vector<std::string> vec;
	std::stringstream ss(fdsf);
	while (std::getline(ss, line, ' '))
	{
		vec.push_back(line);
	}

	if (vec.size() == 9)
	{
        tud_hid_report(0, "true", sizeof("true"));
		global_ui->update_info(vec[0], vec[1], vec[2], vec[3], vec[4], vec[5],
							   vec[6], vec[7], vec[8]);
	} else {
        tud_hid_report(0, "false", sizeof("false"));
    }
}