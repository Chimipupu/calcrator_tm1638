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

void setup()
{
    // UART初期化
    Serial.begin(115200);
    // delay(1000);

    // TM1638初期化
    Serial.printf("[DEBUG]TM1638 Drv Test\n");
    g_tm1638.stb_pin = 4;
    g_tm1638.clk_pin = 3;
    g_tm1638.dio_pin = 2;
    g_tm1638.seg_cnt = 8;
    g_tm1638.key_bit_cnt = 16;
    tm1638_init(g_tm1638);
}

void loop()
{
    tm1638_update();
    delay(1000);
}

