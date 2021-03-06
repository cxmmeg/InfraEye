#include <stdint.h>
#include "module_IRsensor.h"

#include "config.h"
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"

#include "driver/i2c.h"

float IRsensor_fEmissivity = 1.0;
float IRsensor_fReflectedTemp;
static uint16_t IRsensor_au16MLX_Frame[834];
uint8_t IRsensor_u8subPage = 0u;
uint16_t IRsensor_au16EEPROM_Data[832];
paramsMLX90640 IRsensor_sMLX_Parameters;

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

uint8_t IRsensor_u8I2C_Scan();

uint8_t IRsensor_Init(void)
{
	int result = -55;
	uint8_t u8Address;
	int status;
	int error;
	uint16_t u16CtrlReg;

	MLX90640_I2CInit();
	// Scan I2C to make sure IR sensor is present
	//u8Address = IRsensor_u8I2C_Scan();

	// Initialize IR sensor
	/* Keed default resolution = 2*/
	//result = MLX90640_SetResolution(MLX90640_ADR, 0x02);
	ets_printf("Get current resolution\n");
	result = MLX90640_GetCurResolution(MLX90640_ADR);
	ets_printf("Result = %d\n",result);
	ets_printf("Set refresh rate\n");
	MLX90640_SetRefreshRate(MLX90640_ADR, RR16Hz);
	ets_printf("Set interleave mode\n");
	MLX90640_SetInterleavedMode(MLX90640_ADR);
	status = MLX90640_DumpEE (MLX90640_ADR, IRsensor_au16EEPROM_Data);
	status = MLX90640_ExtractParameters(IRsensor_au16EEPROM_Data, &IRsensor_sMLX_Parameters);

    error = MLX90640_I2CRead(MLX90640_ADR, 0x800D, 1, &u16CtrlReg);
    if(error == 0)
    {
    	MLX90640_vSwapBytes(&u16CtrlReg);
    	ets_printf("After init. 0x%x\n",u16CtrlReg);
    }


	return(result);
}

uint8_t IRsensor_u8I2C_Scan()
{
	uint8_t detectedAddress = 0xFF;
	uint8_t address;
	esp_err_t error;
	int nDevices;
	esp_err_t i2c_ret = ESP_OK;
	i2c_cmd_handle_t cmd;
	uint8_t data_h;
	uint8_t data_l;

	ets_printf("Scanning...\n");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
	  cmd = i2c_cmd_link_create();
	  i2c_master_start(cmd);
	  error = i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
	  i2c_master_read_byte(cmd, &data_h, 1);
	  i2c_master_read_byte(cmd, &data_l, 1);
	  i2c_master_stop(cmd);
	  i2c_ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, (1000 / portTICK_RATE_MS));	//"(# / portTICK_RATE_MS)"=maximum wait time. This task will be blocked until all the commands have been sent (not thread-safe - if you want to use one I2C port in different tasks you need to take care of multi-thread issues)
	  i2c_cmd_link_delete(cmd);
//	  ets_printf("0x%.2x|0x%x|%x\t", address, i2c_ret, (data_h<<8)+data_l);

    if (i2c_ret == ESP_OK)
    {
    	ets_printf("Found on address 0x%x...\n",address);
//      Serial.print("I2C device found at address 0x");
//      if (address<16)
//        Serial.print("0");
      detectedAddress = address;
//      Serial.print(address,HEX);
//      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
//      Serial.print("Unknown error at address 0x");
//      if (address<16)
//        Serial.print("0");
//      Serial.println(address,HEX);
    }    
  }
//  if (nDevices == 0)
//    Serial.println("No I2C devices found\n");
//  else
//    Serial.println("done\n");
    return(detectedAddress);
}

uint8_t IRsensor_u8DataIsReady(void)
{
  uint8_t u8DataReady = 0;
  int32_t s32Error;
  uint16_t u16RegisterValue;

  /* Read status register */
  s32Error = MLX90640_I2CRead(MLX90640_ADR, MLX90640_STATUS_REG_ADDR, 1, &u16RegisterValue);
  if(s32Error == 0u)
  {
	  /* Set data ready flag */
	  u8DataReady = u16RegisterValue & 0x0008u;
	  /* Save subpage information */
	  IRsensor_u8subPage = u16RegisterValue & 0x0001u;
  }
  return(u8DataReady);
}

void IRsensor_LoadSubPage(float* mlx90640To)
{
	int status;

	status = MLX90640_GetFrameData_Custom(MLX90640_ADR, IRsensor_au16MLX_Frame, IRsensor_u8subPage);
   MLX90640_I2CWrite(MLX90640_ADR, 0x8000, 0x0010);
  #ifdef LIVE_DATA
  // -------------- Calculate temperature of subframe --------
  IRsensor_fReflectedTemp = MLX90640_GetTa(IRsensor_au16MLX_Frame, &IRsensor_sMLX_Parameters) - TA_SHIFT; //reflected temperature based on the sensor

  /*
  dtostrf((double)tr, 4, 2, str_temp);
  Serial.printf("Ambient temperature: %s�C (%d)\n", str_temp, (int)tr);
   */



  MLX90640_CalculateTo_Custom(IRsensor_au16MLX_Frame, &IRsensor_sMLX_Parameters, IRsensor_fEmissivity, IRsensor_fReflectedTemp, mlx90640To);
  //MLX90640_GetImage(mlx90640Frame, &mlx90640, mlx90640To);
  #endif

   #ifdef TEMPERATURE2CONSOLE
  printTemperaturesToConsole(mlx90640To, 768, 32);
  #endif 
}

void IRsensor_vLoadSubPage_u16(void)
{
    int error;
    uint16_t u16Value;
    uint16_t u16StatusRegister;

    /* Read data from measured subpage */
	(void)MLX90640_GetFrameData_Custom(MLX90640_ADR, IRsensor_au16MLX_Frame, IRsensor_u8subPage);

	/* Clear data ready flag (bit 4) in status register  */
	error = MLX90640_I2CRead(MLX90640_ADR, MLX90640_STATUS_REG_ADDR, 1, &u16StatusRegister);
	if(error == 0)
	{
		u16Value = (u16StatusRegister & 0xFFF7);
		error = MLX90640_I2CWrite(MLX90640_ADR, 0x8000, u16Value);
	}

#ifdef LIVE_DATA

#else
	for(uint16_t i = 0; i < 768; i++)
	{
	pu16PixelValue[i] = (uint16_t)((mlx90640To[i] + (float)TEMP_OFFSET_D) * (float)TEMP_SCALE_FACTOR_D);
	//Serial.printf("%d %d|", pu16PixelValue[i], i);
	//if((i%32)==31) Serial.printf("\n");
	}
#endif

#ifdef TEMPERATURE2CONSOLE
	printTemperaturesToConsole(mlx90640To, 768, 32);
#endif
}

void IRsensor_vCalculatePixelTemp_u16(uint16_t* pu16PixelValue)
{
	/* Calculate reflected (ambient) temperature */
	IRsensor_fReflectedTemp = MLX90640_GetTa(IRsensor_au16MLX_Frame, &IRsensor_sMLX_Parameters) - TA_SHIFT;
	//ets_printf("RT = %d \n",(int32_t)IRsensor_fReflectedTemp);

	MLX90640_CalculateTo_Custom_u16(IRsensor_au16MLX_Frame, &IRsensor_sMLX_Parameters, IRsensor_fEmissivity, IRsensor_fReflectedTemp, pu16PixelValue);
	//ets_printf("T = %d \n",(int32_t)((pu16PixelValue[50]/TEMP_SCALE_FACTOR_D)-TEMP_OFFSET_D));
}

void IRsensor_UpdateMinMax(float* min_t, float* max_t, float* pixelValue)
{
	*min_t = (300+50)*128;
    *max_t = (-40+50)*128;
    for(uint16_t i=0;i<768;i++)
    {
      if(pixelValue[i] > *max_t) *max_t = pixelValue[i];
      if(pixelValue[i] < *min_t) *min_t = pixelValue[i];    
    }
}

void IRsensor_UpdateMinMax_u16(uint16_t* pu16MinTemp, uint16_t* pu16MaxTemp, uint16_t* pu16PixelValue)
{
    *pu16MinTemp = (uint16_t)((300 + TEMP_OFFSET_D) * TEMP_SCALE_FACTOR_D);
    *pu16MaxTemp = (uint16_t)((-40 + TEMP_OFFSET_D) * TEMP_SCALE_FACTOR_D);
    uint8_t u8_MinDif = 10;	// Minimum difference between min and max

    for(uint16_t i = 0; i < 768u; i++)
    {
        if(pu16PixelValue[i] > *pu16MaxTemp)
        {
          *pu16MaxTemp = pu16PixelValue[i];
        } 
        if(pu16PixelValue[i] < *pu16MinTemp) 
        {
          *pu16MinTemp = pu16PixelValue[i];    
        }
    }
    if((*pu16MaxTemp - *pu16MinTemp) < u8_MinDif)
    {
    	*pu16MaxTemp = *pu16MinTemp + u8_MinDif+10;
    }
//	*pu16MinTemp = (uint16_t)((-5 + TEMP_OFFSET_D) * TEMP_SCALE_FACTOR_D);
//	*pu16MaxTemp = (uint16_t)((15 + TEMP_OFFSET_D) * TEMP_SCALE_FACTOR_D);
}

uint8_t IRsensor_u8CurrentSubPage(void)
{
	return(IRsensor_u8subPage);
}
