#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_driver.h"
#include <Wire.h>
#include "module_ImageUpscaling.h"

//#define STMPE_CS 16 // Not used yet
#define TFT_CS   0
#define TFT_DC   2//4//15
// #define SD_CS    0 // Not used yet
#define RAW_IMAGE

//#define TEMPERATURE2CONSOLE   // Debug print of 768 temperatures to console
//#define COLOR2CONSOLE   // Debug print of 768 uint16_t colors to console
#define LIVE_DATA   // defined - sensor data, not defined - static picture

#define MLX90640_ADR 0x33
#define TA_SHIFT 8 //the default shift for a MLX90640 device in open air
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

uint16_t eeMLX90640[832];
paramsMLX90640 mlx90640;
const uint16_t colorPalete[] = {0x1,0x2,0x3,0x4,0x5,0x5,0x6,0x6,0x7,0x7,0x8,0x8,0x9,0x9,0xa,0xa,0xa,0xb,0xb,0xb,0xc,0xc,0xc,0xc,0xd,0x80d,0x80d,0x80e,0x80e,0x80e,0x80e,0x80e,0x80e,0x100f,0x100f,0x100f,0x100f,0x100f,0x180f,0x1810,0x1810,0x1810,0x2010,0x2010,0x2010,0x2010,0x2811,0x2811,0x2811,0x2811,0x3011,0x3011,0x3011,0x3011,0x3811,0x3812,0x3812,0x3812,0x3812,0x3812,0x4012,0x4012,0x4012,0x4012,0x4012,0x4812,0x4812,0x4812,0x4812,0x4812,0x5012,0x5013,0x5013,0x5013,0x5813,0x5813,0x5813,0x5813,0x5813,0x6013,0x6013,0x6013,0x6013,0x6813,0x6813,0x6813,0x6813,0x6813,0x7013,0x7013,0x7013,0x7013,0x7013,0x7813,0x7813,0x7813,0x7813,0x7813,0x8013,0x8013,0x8013,0x8013,0x8013,0x8813,0x8813,0x8813,0x8813,0x8813,0x9013,0x9013,0x9013,0x9013,0x9813,0x9813,0x9813,0x9813,0x9813,0x9813,0xa013,0xa013,0xa013,0xa013,0xa013,0xa013,0xa813,0xa813,0xa813,0xa813,0xa813,0xa813,0xa813,0xb013,0xb013,0xb012,0xb012,0xb012,0xb012,0xb012,0xb012,0xb012,0xb812,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xc032,0xc032,0xc052,0xc052,0xc051,0xc051,0xc051,0xc071,0xc071,0xc071,0xc071,0xc071,0xc891,0xc890,0xc890,0xc890,0xc890,0xc8b0,0xc8b0,0xc8b0,0xc8b0,0xc8cf,0xc8cf,0xc8cf,0xd0cf,0xd0cf,0xd0ee,0xd0ee,0xd0ee,0xd0ee,0xd10e,0xd10d,0xd10d,0xd10d,0xd12d,0xd12c,0xd12c,0xd92c,0xd94c,0xd94c,0xd94b,0xd96b,0xd96b,0xd96a,0xd96a,0xd98a,0xd989,0xd989,0xd988,0xd9a8,0xd9a8,0xd9a7,0xe1a7,0xe1c6,0xe1c6,0xe1c6,0xe1c5,0xe1e5,0xe1e4,0xe1e4,0xe1e4,0xe203,0xe203,0xe203,0xe223,0xe223,0xe222,0xe222,0xe242,0xe242,0xea42,0xea62,0xea61,0xea61,0xea61,0xea61,0xea61,0xea81,0xea81,0xea81,0xea81,0xeaa1,0xeaa1,0xeaa1,0xeac1,0xeac0,0xeac0,0xeac0,0xeae0,0xeae0,0xeae0,0xeae0,0xeae0,0xeb00,0xeb00,0xeb00,0xf300,0xf320,0xf320,0xf320,0xf320,0xf320,0xf340,0xf340,0xf340,0xf340,0xf340,0xf360,0xf360,0xf360,0xf360,0xf380,0xf380,0xf380,0xf380,0xf3a0,0xf3a0,0xf3a0,0xf3a0,0xf3c0,0xf3c0,0xf3c0,0xf3e0,0xf3e0,0xf3e0,0xf400,0xf400,0xf400,0xf400,0xf420,0xf420,0xf420,0xfc20,0xfc40,0xfc40,0xfc40,0xfc40,0xfc40,0xfc60,0xfc60,0xfc60,0xfc60,0xfc60,0xfc80,0xfc80,0xfc80,0xfc80,0xfca0,0xfca0,0xfca0,0xfcc0,0xfcc0,0xfcc0,0xfce0,0xfce0,0xfce0,0xfd00,0xfd00,0xfd00,0xfd00,0xfd20,0xfd20,0xfd20,0xfd40,0xfd40,0xfd40,0xfd60,0xfd60,0xfd60,0xfd60,0xfd80,0xfd80,0xfd80,0xfd80,0xfda0,0xfda0,0xfda0,0xfdc0,0xfdc0,0xfdc0,0xfdc0,0xfdc0,0xfde0,0xfde0,0xfde0,0xfe00,0xfe00,0xfe00,0xfe00,0xfe20,0xfe20,0xfe20,0xfe20,0xfe40,0xfe40,0xfe40,0xfe40,0xfe40,0xfe60,0xfe60,0xfe60,0xfe60,0xfe60,0xfe80,0xfe80,0xfe81,0xfea1,0xfea1,0xfea1,0xfea1,0xfec1,0xfec1,0xfec1,0xfec1,0xfec2,0xfee2,0xfee2,0xfee2,0xfee3,0xfee3,0xfee3,0xff04,0xff04,0xff04,0xff04,0xff05,0xff25,0xff25,0xff26,0xff26,0xff27,0xff27,0xff47,0xff48,0xff48,0xff49,0xff49,0xff6a,0xff6a,0xff6a,0xff6b,0xff6b,0xff6c,0xff6c,0xff8d,0xff8d,0xff8e,0xff8e,0xff8f,0xff90,0xff90,0xff91,0xff91,0xffb2,0xffb2,0xffb3,0xffb3,0xffb4,0xffb4,0xffb5,0xffb5,0xffb6,0xffb6,0xffd7,0xffd7,0xffd8,0xffd8,0xffd8,0xffd9,0xffd9,0xffda,0xffda,0xffdb,0xfffb,0xfffb,0xfffc,0xfffc,0xfffd,0xfffd,0xfffd,0xfffe,0xfffe,0xfffe};
#ifndef LIVE_DATA
float mlx90640To[768] = {
26.828890, 26.369379, 24.731531, 22.822830, 21.742044, 22.154196, 21.248055, 18.652376, 21.279518, 21.589119, 21.223455, 19.639475, 21.423983, 22.051989, 25.295067, 25.472538, 30.299950, 31.372770, 32.081581, 29.557137, 25.280773, 24.964289, 22.820082, 19.773882, 21.649080, 22.629297, 23.314922, 21.471209, 22.833958, 23.748293, 23.172810, 21.104683, 
21.831778, 22.130459, 19.042555, 17.647865, 21.836405, 21.595093, 19.790569, 18.341248, 22.781235, 22.384573, 20.279596, 18.823023, 22.693974, 22.993046, 25.177101, 27.142065, 35.421604, 35.911507, 34.589447, 32.406796, 27.066525, 25.756014, 22.615564, 20.768702, 23.795815, 23.446800, 20.906960, 19.944033, 24.462500, 24.453224, 20.964813, 19.996763, 
27.617880, 27.547131, 25.577343, 23.458559, 23.608805, 23.738573, 22.861874, 20.991133, 21.590612, 22.319729, 21.519245, 20.288164, 21.938269, 22.815395, 25.927713, 26.551786, 33.361221, 33.240173, 32.115917, 29.616325, 25.323372, 25.363428, 23.786224, 21.434532, 22.344526, 22.720999, 23.137835, 20.769524, 23.419613, 23.431179, 23.695917, 21.461056, 
21.577135, 21.479715, 19.612616, 18.595718, 22.484760, 22.702475, 19.621696, 19.272942, 22.430035, 22.291265, 20.665966, 18.992407, 23.414009, 23.056702, 23.306482, 23.195303, 28.708477, 28.276463, 25.530321, 24.051512, 25.669617, 25.659992, 22.920307, 21.607004, 23.855467, 23.610523, 20.874630, 20.117409, 24.126226, 23.793415, 21.677525, 20.007483, 
24.867500, 25.471888, 24.946301, 24.123703, 24.825264, 25.203203, 23.877615, 21.596207, 21.600546, 22.079237, 22.191612, 20.799837, 22.582022, 23.398123, 24.925753, 23.685278, 25.209145, 25.070082, 25.309486, 23.899853, 24.551970, 24.923872, 24.072807, 21.850048, 22.184397, 22.358078, 23.176424, 21.848881, 23.256748, 23.214144, 23.675724, 22.016596, 
21.788233, 21.359074, 19.114090, 18.423634, 22.602751, 22.405865, 20.584120, 19.035360, 22.227936, 22.511057, 21.097925, 19.883083, 23.723419, 23.675955, 23.144758, 22.362200, 25.955065, 26.064960, 24.152426, 22.863113, 26.016756, 25.597902, 23.783110, 22.748571, 24.568893, 23.686531, 21.906328, 19.995447, 24.480017, 24.844000, 21.886648, 20.366426, 
24.259626, 24.499144, 25.130896, 23.514795, 24.498817, 25.328869, 23.893723, 22.166964, 22.222279, 21.993816, 22.112125, 20.367895, 22.598108, 23.284250, 24.576271, 23.457504, 25.031855, 25.461386, 25.375088, 24.519110, 25.087902, 25.502867, 24.867786, 22.719265, 22.729301, 23.384613, 23.372484, 21.379526, 23.066614, 24.210711, 24.141651, 22.501038, 
21.436310, 21.868689, 19.269020, 17.798756, 21.980656, 22.529984, 20.279840, 18.940369, 22.553160, 22.351734, 20.999285, 20.182579, 23.954594, 24.024988, 23.248343, 22.269444, 26.449381, 26.281792, 24.925123, 24.022062, 26.446201, 26.071545, 24.325933, 22.862175, 24.451365, 24.481842, 21.977190, 20.801500, 24.020296, 24.517397, 23.119930, 20.668560, 
24.188349, 24.828693, 25.445444, 23.759203, 25.437696, 25.340530, 23.920752, 22.063850, 22.185625, 22.311596, 22.153942, 21.063116, 22.254841, 23.192200, 24.664864, 24.040140, 25.303753, 25.659903, 25.650715, 25.189732, 25.968596, 25.823931, 25.132776, 23.605984, 23.174408, 23.761992, 23.561863, 22.308111, 24.208164, 24.922518, 24.594303, 22.723900, 
21.567480, 21.611103, 19.433153, 18.309788, 21.896944, 22.186090, 19.991335, 19.217539, 22.783625, 22.385580, 20.659395, 20.175316, 23.472729, 23.623844, 23.399343, 22.498795, 26.118862, 26.464121, 25.631788, 24.735502, 27.339569, 27.622608, 24.836657, 23.634882, 24.501347, 24.259560, 22.163450, 21.514923, 24.928787, 25.225323, 22.339104, 21.186684, 
24.193968, 25.218485, 25.228897, 23.498911, 24.172760, 24.526413, 24.176136, 22.167128, 22.601192, 22.751379, 22.216997, 20.822849, 22.553408, 23.279179, 24.635998, 23.594530, 25.980537, 26.851974, 28.612789, 27.363646, 28.409624, 28.254477, 28.390158, 26.356989, 24.393286, 24.325808, 24.171066, 22.778976, 25.173668, 25.025324, 25.327154, 23.383976, 
21.452597, 21.471428, 18.885572, 18.415546, 22.479568, 21.982912, 20.045189, 19.164417, 22.169102, 22.564613, 20.931873, 19.942116, 23.321564, 23.832281, 23.693357, 23.483669, 28.209999, 29.180017, 28.595087, 27.880363, 30.272581, 30.152214, 27.647444, 25.865871, 25.499365, 25.164078, 22.646450, 21.935070, 25.693869, 25.630896, 22.986097, 22.381840, 
24.249506, 24.700451, 24.543110, 22.757988, 24.314764, 24.651722, 23.836479, 21.876699, 22.286398, 22.696875, 21.901907, 20.652300, 22.650328, 23.238766, 25.638353, 25.607969, 32.112839, 33.425999, 35.347542, 35.713905, 38.532001, 39.201561, 41.684196, 38.948818, 26.423088, 25.791611, 24.871141, 23.498871, 25.245510, 25.381329, 25.350798, 23.615150, 
22.124016, 21.943121, 20.484701, 17.975782, 22.476414, 22.160414, 20.332987, 18.918695, 22.022589, 22.180376, 20.699957, 19.683496, 23.084249, 23.479977, 24.649414, 25.493496, 38.979954, 41.920494, 45.364605, 45.944183, 50.357723, 50.214306, 42.751633, 36.804893, 26.912134, 26.437262, 23.114683, 22.081635, 25.535336, 25.624710, 23.014208, 22.483982, 
24.325132, 24.533222, 24.656359, 22.804085, 24.554827, 23.974091, 23.707645, 22.124975, 22.341511, 22.402214, 21.847515, 19.875778, 21.907000, 22.176352, 24.540335, 26.877106, 49.490528, 54.399776, 59.335129, 57.985424, 58.003212, 58.014709, 54.235035, 46.228497, 27.516987, 26.435591, 25.951830, 24.255432, 26.079102, 26.389961, 25.665932, 23.989143, 
22.094019, 21.465872, 20.289110, 18.243612, 21.682823, 21.781273, 19.768839, 18.801159, 21.890879, 21.788727, 20.220556, 19.440538, 23.202650, 23.434980, 23.255377, 24.174820, 49.226093, 56.313328, 60.839642, 59.543106, 60.043713, 57.827995, 46.781620, 38.441151, 28.239870, 27.160229, 24.469656, 23.331879, 26.961693, 27.072470, 25.084419, 23.058277, 
23.374992, 24.037643, 24.380272, 22.774822, 23.865379, 24.477491, 24.177914, 22.451221, 22.762167, 22.686295, 22.123791, 19.916418, 22.196526, 22.379807, 23.457087, 23.331614, 42.313309, 52.712734, 63.668861, 61.770725, 58.887012, 56.985538, 50.227055, 41.088707, 28.390564, 28.253929, 28.462955, 26.988209, 28.324474, 28.226198, 27.629536, 26.334513, 
22.363787, 22.306990, 19.601862, 18.904705, 22.164747, 21.513601, 19.407623, 18.753387, 21.868254, 22.877876, 19.977097, 19.976254, 22.626736, 23.234583, 21.832476, 22.530014, 40.752518, 51.776558, 63.046608, 59.865784, 59.706722, 52.680977, 38.690723, 31.986732, 28.792809, 29.067179, 27.678286, 27.123600, 29.529398, 29.862732, 26.584862, 25.523441, 
25.788994, 25.077860, 24.684347, 22.957970, 23.679565, 23.655874, 23.512741, 21.906841, 22.442600, 22.690596, 22.293520, 20.767124, 21.953217, 22.021299, 22.728279, 22.315596, 35.885357, 44.108494, 47.221581, 41.758659, 37.502029, 35.840172, 31.786209, 27.458994, 27.494131, 27.598518, 29.785591, 28.398716, 30.247927, 29.720470, 30.318666, 28.355423, 
30.424034, 30.050646, 24.321596, 20.946110, 22.284586, 22.205706, 20.579937, 18.791243, 22.237242, 22.497818, 20.614120, 20.221315, 23.274622, 23.316912, 22.047609, 21.075289, 32.836754, 36.956810, 35.316505, 32.455360, 31.911762, 30.955378, 27.005478, 24.988022, 27.882156, 27.889301, 27.705894, 27.507870, 30.066772, 30.718523, 28.478922, 26.951866, 
30.340496, 29.678480, 27.970255, 24.898495, 24.100307, 23.479826, 23.504858, 22.023678, 23.299215, 22.521557, 23.252300, 21.350119, 23.144936, 23.351170, 24.770908, 23.300537, 26.580912, 27.455225, 28.335470, 26.845959, 28.082905, 28.104584, 27.306040, 25.746284, 27.726637, 27.307772, 27.533821, 26.045254, 28.140633, 28.476492, 29.346539, 26.995102, 
31.550095, 31.951963, 28.703684, 27.053297, 27.807213, 25.435900, 21.031059, 19.539782, 23.847025, 23.959515, 22.463680, 21.606813, 26.300331, 26.634722, 25.534317, 24.141661, 28.590931, 28.763512, 26.954666, 26.468189, 29.167185, 29.009499, 26.093744, 24.864256, 27.836006, 28.547028, 26.457615, 24.216362, 28.758507, 28.988863, 26.669720, 25.100882, 
31.076130, 31.461153, 31.157354, 28.858707, 29.057129, 28.186031, 25.714796, 24.460085, 25.592718, 25.568495, 25.992062, 25.216057, 27.997517, 28.264540, 29.201563, 27.225447, 29.291323, 28.971027, 29.465622, 27.456417, 28.571686, 28.709888, 28.473419, 27.421230, 27.778091, 27.508280, 27.598213, 25.734013, 27.647085, 26.738651, 26.560688, 24.200018, 
32.088753, 30.563900, 28.151869, 27.495949, 31.573385, 30.869184, 26.378063, 22.739149, 25.992474, 25.923962, 25.064892, 25.197500, 29.335503, 29.400650, 27.571798, 26.694941, 30.337997, 29.997389, 28.669714, 27.251089, 31.289461, 32.086872, 30.189955, 28.874784, 28.749487, 28.855423, 25.507624, 24.638683, 28.104958, 28.932964, 24.717104, 23.813780};
#endif

void setup()
{
  int result = -55;
  uint16_t data;
  uint8_t address;
  int status; 

  // Initialize serial port for debugging
  Serial.begin(115200);
  // Initialize LCD
  tft.begin();
  tft.fillRect(0, 0, 240, 320, 0x00031);
  // Initialize I2C
  Wire.begin();  
  Wire.setClock(800000);
  Serial.println("\nInfraEye boot...");
  // Scan I2C to make sure IR sensor is present
  address = I2C_scan();
  // Initialize IR sensor
  MLX90640_SetResolution(MLX90640_ADR, 0x03);
  Serial.println("Get MLX90640 resolution:");
  result = MLX90640_GetCurResolution(0x33);
  Serial.println(result);
  MLX90640_SetRefreshRate(MLX90640_ADR, 0x03);
  MLX90640_SetInterleavedMode(MLX90640_ADR);  
  status = MLX90640_DumpEE (MLX90640_ADR, eeMLX90640);
  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640); 
}

void loop(void)
{
  int result = -55;
  uint16_t data;
  char str_temp[6];
  float emissivity = 0.95;
  float tr;
  static uint16_t mlx90640Frame[834];
  #ifdef LIVE_DATA
  static float mlx90640To[768];
  #endif

  static float afUpscaleBuffer[OUTPUT_BUFFER_SIZE_D]; 
  int status;
  static uint16_t frameColor[OUTPUT_BUFFER_SIZE_D]; // scaled
  static uint16_t frameColor1[768];                 // not scaled
  float min_t = 300;
  float max_t = -40;
  static float FPS = 0;
  unsigned long time_start, time_1, time_2, time_3, time_4, time_5, time_end;
  uint8_t subPage;
  uint8_t dataReady = 0;
  uint16_t registerValue;
  uint8_t error;
    uint16_t u16DisplayIterator;
  uint16_t u16DispCoordinateX, u16DispCoordinateY;


  // -------------- Wait for new subpage- --------------------
  time_start = micros();
  while(dataReady == 0)
  {
    error = MLX90640_I2CRead(MLX90640_ADR, 0x8000, 1, &registerValue);
    if(error != 0)
    {
      Serial.printf("Error while reading status register. Error code:%d\n", error);      
    }    
    dataReady = registerValue & 0x0008;
    wdt_reset();
  }
  subPage = registerValue & 0x0001;
  Serial.printf("Subpage %d\n", subPage);
  //error = MLX90640_I2CWrite(MLX90640_ADR, 0x8000, (registerValue && ~0x0008));
  //if(error != 0)
  {
    //Serial.printf("Error while reading status register. Error code:%d\n", error);      
  }    
  time_1 = micros();

  // -------------- Read subframe ----------------------------
  status = MLX90640_GetFrameData(MLX90640_ADR, mlx90640Frame);
  //Serial.printf("Status:%d\n", status);
  time_2 = micros();

  #ifdef LIVE_DATA
  // -------------- Calculate temperature of subframe --------
  tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
  dtostrf((double)tr, 4, 2, str_temp);
  Serial.printf("Ambient temperature: %s°C (%d)\n", str_temp, (int)tr);
  wdt_reset();
  MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
  #endif
  
  #ifdef TEMPERATURE2CONSOLE
  printTemperaturesToConsole(mlx90640To, 768, 32);
  #endif
  
  time_3 = micros();
  
  // -------------- Wait for RR - 20 % -----------------------
  for(uint16_t i=0;i<768;i++)
  {
    if(mlx90640To[i] > max_t) max_t = mlx90640To[i];
    if(mlx90640To[i] < min_t) min_t = mlx90640To[i];    
  }
  // -------------- Upscale ----------------------------------
  img_up_vResetUpscaling();
  u16DisplayIterator = 0;
  
  for (uint16_t u16Iterator = 0; u16Iterator < (OUTPUT_NUM_OF_PIXELS_D / OUTPUT_BUFFER_SIZE_D); u16Iterator++)
  {
    
    // -------------- Upscale temperature image ---------------
    img_up_vUpscaleImage(mlx90640To, afUpscaleBuffer, OUTPUT_BUFFER_SIZE_D);
  
    // -------------- Convert temperature to color code -------

    Convert(afUpscaleBuffer, frameColor, OUTPUT_BUFFER_SIZE_D, colorPalete, 403, min_t, max_t, subPage);
        
    for(uint16_t i = 0; i < OUTPUT_BUFFER_SIZE_D; i++)
    {
      u16DispCoordinateY = u16DisplayIterator / OUTPUT_ARRAY_LENGTH_D;
      //if((u16DispCoordinateY % 2) == subPage)
      {
        u16DispCoordinateX = OUTPUT_ARRAY_LENGTH_D - (u16DisplayIterator % OUTPUT_ARRAY_LENGTH_D);
        //u16DispCoordinateX = ((OUTPUT_ARRAY_LENGTH_D - 1) - (u16DisplayIterator % OUTPUT_ARRAY_LENGTH_D));
        /* Scaling factor is 4 - display pixel as 2x2 square */
        tft.fillRect(2 * u16DispCoordinateY, 2* u16DispCoordinateX, 2, 2, frameColor[i]);
        //tft.drawPixel(u16DispCoordinateY, u16DispCoordinateX, frameColor[i]);
      }
      
      u16DisplayIterator++;

      
    }
  }
  time_4 = micros();
  wdt_reset();

#if 0
  Convert(mlx90640To, frameColor1, 768, colorPalete, 403, min_t, max_t, subPage);
  #ifdef COLOR2CONSOLE
  printColorToConsole(frameColor1, 768, 32);
  #endif
  drawImage(frameColor1, 32, 24, 120, 0, 4, 0);
#endif
  
  tft.fillRect(0, 200, 240, 80, 0x0000);
  tft.setCursor(10, 200);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  dtostrf((double)max_t, 3, 0, str_temp);
  tft.printf("Tmax=%s°C", str_temp);
  tft.setCursor(10, 240);
  dtostrf((double)min_t, 3, 0, str_temp);
  tft.printf("Tmin=%s°C", str_temp);
  dtostrf((double)FPS, 2, 1, str_temp);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.setCursor(10, 270);
  tft.printf("%sFPS", str_temp);
  time_5 = micros();
//  MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
//  MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);
  time_end = micros();
  FPS = (float)1000000 / ((float)time_end-(float)time_start);
  Serial.printf("Waiting for data ready%d us\tGetFrame %dus\tCalculateTemperature %dus\nUpscale, Convert, write do LCD %dus\tDisplay text %dus\tTotal period %dms\n", time_1-time_start, time_2-time_1, time_3-time_2, time_4-time_3, time_5-time_4, time_end-time_5, (time_end-time_start)/1000);
}

void drawImage(uint16_t *colorArray, uint16_t columns, uint16_t rows, uint16_t x0, uint16_t y0, uint8_t pixelSize, uint8_t debug)
{
  char str_temp[6];
  uint16_t x, y;
  //tft.fillRect(0, 0, 8, 8, 0xFFFF);
  for(uint16_t i=0;i<columns*rows;i++)
  {
    x = (columns-(i%columns));    
    y = (i/columns);
    
  //if((((subPage+i+(y%2))%2)==1))

      if(debug==1)
      {
        if((i%columns)==0)
        {
          Serial.printf("\n");
          wdt_reset();
        }
        //dtostrf(colorArray[i], 2, 0, str_temp);  
        //Serial.printf("%s;", str_temp);
        Serial.printf("%.2x;", colorArray[i]);
      }
      Serial.printf("|x%dy%dc%x| ", x, y, colorArray[i]);      
      if(pixelSize==1)
      {
        tft.drawPixel(x+x0, y+y0, colorArray[i]);
      }else
      {
        tft.fillRect(x*pixelSize+x0, y*pixelSize+y0, pixelSize, pixelSize, colorArray[i]);
      }
        
  }
}
  
void Convert(float *frameTemperature, uint16_t *frameColor, uint16_t frameSize,
  const uint16_t *colorScale, uint16_t colorScaleSize,
  float minTemperature, float maxTemperature, uint8_t subPage)
{
  uint16_t i;
  //uint16_t constDegToScale;
  float constDegToScale;
  uint16_t colorIndex;

  if(minTemperature>15) minTemperature = 15;
  if(maxTemperature<50) maxTemperature = 50;

  constDegToScale = ((float)maxTemperature - (float)minTemperature) / ((float)colorScaleSize - (float)1);
//  Serial.printf("constDegToScale=%d maxTemperature=%d minTemperature=%d colorScaleSize=%d\n", (int)constDegToScale, maxTemperature, minTemperature, colorScaleSize);

  for(i=0; i<frameSize; i++)
  {
    if(((i/32)%2)==subPage)
    {
      if((frameTemperature[i] >= minTemperature) && (frameTemperature[i] <= maxTemperature))
      {
        colorIndex = (uint16_t)((frameTemperature[i] - (float)minTemperature) / constDegToScale);
      }else
      {
        colorIndex = 0;
      }
      //Serial.printf("%.4x", (int)frameTemperature[i]);
      //if((i%32)==0)
      //{
      //  Serial.printf("\n");
      //  wdt_reset();
      //}
      frameColor[i] = colorScale[colorIndex];
    }
  }
}

unsigned long readSensor(int value) {
/*  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("MLX90640");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(value);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(value, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");*/  
  return micros();// - start;
}

uint8_t I2C_scan(void)
{
  uint8_t detectedAddress = 0xFF;
    byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      detectedAddress = address;
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
    return(detectedAddress);
}

void printTemperaturesToConsole(float *mlx90640To, uint16_t pixelCount, uint8_t rowLength)
{
  for(uint16_t i=0;i<768;i++)
  {
    if((i%32)==0) Serial.printf("\n");
    Serial.printf("%f, ", mlx90640To[i]);
  }
  Serial.printf("\n");
}

void printColorToConsole(uint16_t *color, uint16_t pixelCount, uint8_t rowLength)
{
  for(uint16_t i=0;i<768;i++)
  {
    if((i%32)==0) Serial.printf("\n");
    Serial.printf("%x ", color[i]);
  }
  Serial.printf("\n");
}
