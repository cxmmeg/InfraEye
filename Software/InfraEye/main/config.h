
// General
//#define TEMPERATURE2CONSOLE   // Debug print of 768 temperatures to console
//#define COLOR2CONSOLE   // Debug print of 768 uint16_t colors to console
#define LIVE_DATA   // defined - sensor data, not defined - static picture

// IR sensor Address
#define MLX90640_ADR 			0x33
//the default shift for a MLX90640 device in open air
#define TA_SHIFT 				8.0
enum RefreshRate
{
	RR0_5Hz,
	RR1Hz,
	RR2Hz,
	RR4Hz,
	RR8Hz,
	RR16Hz,
	RR32Hz,
	RR64Hz
};

#define REFRESH_RATE	RR2Hz

// LCD
#define LCD_PIN_NUM_MISO 25
#define LCD_PIN_NUM_MOSI 23
#define LCD_PIN_NUM_CLK  19
#define LCD_PIN_NUM_CS   22

#define LCD_PIN_NUM_DC   21
#define LCD_PIN_NUM_RST  18
//#define LCD_PIN_NUM_BCKL 5

#define RAW_IMAGE

// Touch
#define TOUCH_PIN_NUM_MISO	33
#define TOUCH_PIN_NUM_MOSI	32
#define TOUCH_PIN_NUM_CLK	5
#define TOUCH_PIN_NUM_CS	15

// SD

