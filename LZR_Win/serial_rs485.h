#ifndef SERIAL_RS485_H_
#define SERIAL_RS485_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HEADER_SYNC_VAL			0xFFFEFDFC

#define MIN_MSG_SIZE							8					// SYNC(4) + SIZE(2) + FOOTER(2)
#define HEADER_SYNC_SIZE					4
#define HEADER_MESSAGE_SIZE			2
#define MESSAGE_CMD_SIZE					2
#define FOOTER_CHK_SIZE					2

	typedef struct LZR920
	{
		unsigned int header_sync;					// 0xFFFEFDFC
		unsigned short header_size;				// message의 크기
		unsigned short message_cmd;				// 메시지 아이디
		unsigned char* message_data;			// 데이터
		unsigned short footer_chk;
	} ST_LZR920;

#ifdef _WIN32
#pragma pack(push,1)
	typedef struct DataConfig_50004
#else
	typedef struct __attribute__((packed)) DataConfig_50004
#endif
	{
		// D0-D3 : status bits (비트단위로 확인 필요한 상태메시지)
		unsigned char D0_status0;
		unsigned char D1_status1;
		unsigned char D2_status2;
		unsigned char D3_status3;
		// D4-D5 : communication charge (in %)
		unsigned short D4D5_comm_charge;
		// D6 : Baud rate [0=57600, 1=115200, 2=230400, 3=460800, 4=921600]
		unsigned char D6_baud_rate;
		// D7 : reserved (0)
		unsigned char D7_nodata0;
		// D8 : LZR infos enabled [0]disabled [1]enabled
		unsigned char D8_LZR_information;
		// D9 : Red laser timeout
		unsigned char D9_Red_laser_timeout;
		// D10 : Enable/disable test frame [0]disabled [1]enabled
		unsigned char D10_test_frame_enable;
		// D11 : Enable/disable plane 0 [0]disabled [1]enabled
		unsigned char D11_plane0_enable;
		// D12 : Enable/disable plane 1 [0]disabled [1]enabled
		unsigned char D12_plane1_enable;
		// D13 : Enable/disable plane 2 [0]disabled [1]enabled
		unsigned char D13_plane2_enable;
		// D14 : Enable/disable plane 3 [0]disabled [1]enabled
		unsigned char D14_plane3_enable;
		// D15 : Enable/disable pulse width [0]disabled [1]enabled
		unsigned char D15_pulse_width_enable;
		// D16~D17 : Number of distance values [1-274]
		unsigned short D1617_number_distance_values;
		// D18~D19 : Starting spot number [0-273]
		unsigned short D1819_starting_spot;
		// D20~D21 :  Jump between 2 successive spots [0-273]
		unsigned short D2021_gap_between_spots;
		// D22 : Apd distance range [0=8m, 1=12m, 2=16m]
		unsigned char D22_apd_distance_range;
		// D23 : Enable/disable transmission of the can id and frame counter [0]disabled [1]enabled
		unsigned char D23_can_id_frame_counter_enable;
		// D24 : Enable/disable the diode lifetime management [0]disabled [1]enabled
		unsigned char D24_diode_lifetime_management_enable;
		// D25 : Polarity of the input 1 [0]high [1]low
		unsigned char D25_polarity_input1;
		// D26 : Delay of 2 heartbeat messages (unit = 1 sec) [0-255]
		unsigned char D26_heartbeat_delay_second;
		// D27 : Enable/disable led 1 [0]disabled [1]enabled
		unsigned char D27_led1_enable;
		// D28 : Enable/disable led 2 [0]disabled [1]enabled
		unsigned char D28_led2_enable;
		// D29 : Enable/disable led blue [0]disabled [1]enabled
		unsigned char D29_led_blue_enable;
		// D30 : Enable/disable led error [0]disabled [1]enabled
		unsigned char D30_led_error_enable;
		// D31 : Duration of LEDs at boot-up [0-255] [0]managed through input 2 [1-254] unit 1sec [255] always on
		unsigned char D31_led_boot_duration;
		// D32~33 : Maximum distance range SW [0~65000] ...현재 레이저는 16000 이상으로는 의미없음
		unsigned short D3233_max_distance_range_SW;
		// D34 : Enable/disable plan number inside the frame [0]disabled [1]enabled
		unsigned char D34_plane_number_inside_frame_enable;
		// D35 : Immunity level [1-4]
		unsigned char D35_immunity_level;
		// D36~37 : Hot reset timer (unit = 1 sec)
		unsigned short D3637_hot_reset_timer_second;
		// D38 : Hot reset counter [0]disable [1~255]number of reset
		unsigned char D38_hot_reset_counter;
	} ST_DataConfig;
#ifdef _WIN32
#pragma pack(pop)
#endif

#ifdef _WIN32
#pragma pack(push,1)
	typedef struct stSETRAWDATACONFIG_50003
#else
	typedef struct __attribute__((packed)) stSETRAWDATACONFIG_50003
#endif
	{
		unsigned char  D0_baud_rate;                // 0=57600,1=115200,2=230400,3=460800,4=921600
		unsigned char  D1_reserved;                 // 0
		unsigned char  D2_LZR_infos_enable;         // 0=disable,1=enable
		unsigned char  D3_red_laser_timeout;        // 0~255 (unit=15s)
		unsigned char  D4_test_frame_enable;        // 0/1
		unsigned char  D5_plane0_enable;            // 0/1
		unsigned char  D6_plane1_enable;            // 0/1
		unsigned char  D7_plane2_enable;            // 0/1
		unsigned char  D8_plane3_enable;            // 0/1
		unsigned char  D9_pulse_width_enable;       // 0/1
		unsigned short D10_11_number_distance_values; // [1–274]
		unsigned short D12_13_starting_spot;          // [0–273]
		unsigned short D14_15_gap_between_spots;      // [0–273]
		unsigned char  D16_apd_distance_range;      // 0=8m,1=12m,2=16m
		unsigned char  D17_canid_fc_enable;         // 0/1
		unsigned char  D18_diode_lifetime_enable;   // 0/1
		unsigned char  D19_polarity_input1;         // 0=high,1=low
		unsigned char  D20_heartbeat_delay;         // sec [0–255]
		unsigned char  D21_led1_enable;             // 0/1
		unsigned char  D22_led2_enable;             // 0/1
		unsigned char  D23_led_blue_enable;         // 0/1
		unsigned char  D24_led_error_enable;        // 0/1
		unsigned char  D25_led_boot_duration;       // [0–255]
		unsigned short D26_27_max_distance_range_SW; // [0–65000]
		unsigned char  D28_plane_number_in_frame;   // 0/1
		unsigned char  D29_immunity_level;          // [1–4]
		unsigned short D30_31_hot_reset_timer;      // sec
		unsigned char  D32_hot_reset_counter;       // 0/1–255
	} ST_SETRAWDATACONFIG_50003;
#ifdef _WIN32
#pragma pack(pop)
#endif


	int openSerialPort(char* portName, int baudRate);
	void closeSerialPort(void);
	void initConfigData(ST_SETRAWDATACONFIG_50003* p_stData);

	// 메시지 생성부
	void request_MeasurementMode(void);
	void request_ConfigMode(void);
	void request_GetConfig(void);				// 현재 설정값 조회
	void request_changeSetting(ST_SETRAWDATACONFIG_50003 stData);
	void startTimerRequestConfigurationMode(void);			// 설정모드 요청 타이머
	void stopTimerRequestConfigurationMode(void);			// 설정모드 요청 타이머 중지
	void request_CurrentMode(void);			// 현재 모드 조회
	long long sendPacket(unsigned char* _pPacket, unsigned long long _sizePacket);
	void request_RestoreSetting(void);
	void request_saveConfig_EEPROM(void);				// 설정값 영구저장

#ifdef __cplusplus
}
#endif

#endif