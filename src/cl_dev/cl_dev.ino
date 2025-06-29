/**
 * @file cl_dev.ino
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief Arduino IDE用のファイル
 * @version 0.1
 * @date 2025-06-27
 * 
 * @copyright Copyright (c) 2025 Chimipupu(https://github.com/Chimipupu)
 * 
 */

#include "drv_tm1638.hpp"

tm1638_t g_tm1638;
static uint8_t s_key = 0;

void setup()
{
    // UART初期化
    Serial.begin(115200);

    // TM1638初期化
#if 0
    // Arduino Nano
    g_tm1638.stb_pin = 9;
    g_tm1638.clk_pin = 10;
    g_tm1638.dio_pin = 11;
#else
    // Raspberry Pi Pico 2
    g_tm1638.stb_pin = 4;
    g_tm1638.clk_pin = 3;
    g_tm1638.dio_pin = 2;
    Serial.printf("[DEBUG]TM1638 Drv Test\n");
#endif
    g_tm1638.seg_cnt = 8;
    g_tm1638.key_bit_cnt = 16;
    tm1638_init(g_tm1638);
}

void loop()
{
    uint8_t key = 0;

    // 7セグの表示を更新
#if 0
    static uint32_t s_num = 0;
    tm1638_uint32_to_7seg(s_num);
    if (s_num >= SEG_U32_MAX) {
        s_num = 0;
    } else {
        s_num++;
    }
#else
    static float s_fp_num = 3.141592;
    tm1638_float_to_7seg(s_fp_num);
#endif

#if 0
    // キースキャン
    key = tm1638_read_key();
    if (key != s_key) {
        Serial.printf("[DEBUG]TM1638 Key = 0x%02\n", key);
    }
    s_key = key;
#endif

    // delay(100);
}