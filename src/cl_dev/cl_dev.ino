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

void shitf_out(uint8_t data)
{
    shiftOut(g_tm1638.dio_pin, g_tm1638.clk_pin, LSBFIRST, data);
}

void setup()
{
    uint8_t i;

    // UART初期化
    Serial.begin(115200);

    // TM1638初期化
    g_tm1638.stb_pin = 9;
    g_tm1638.clk_pin = 10;
    g_tm1638.dio_pin = 11;
    g_tm1638.seg_cnt = 8;
    g_tm1638.key_bit_cnt = 16;

#if 1
    tm1638_init(g_tm1638);
#else
    pinMode(g_tm1638.stb_pin, OUTPUT);
    pinMode(g_tm1638.clk_pin, OUTPUT);
    pinMode(g_tm1638.dio_pin, OUTPUT);
    digitalWrite(g_tm1638.stb_pin, HIGH);
    digitalWrite(g_tm1638.clk_pin, HIGH);
    digitalWrite(g_tm1638.dio_pin, LOW);

    // TM1638にディスプレイ有効を設定
    digitalWrite(g_tm1638.stb_pin, LOW);
    shitf_out(TM1638_CMD_WRITE_DISPLAY_REG);
    digitalWrite(g_tm1638.stb_pin, HIGH);

    digitalWrite(g_tm1638.stb_pin, LOW);
    shitf_out(TM1638_CMD_DISPLAY_ON);
    digitalWrite(g_tm1638.stb_pin, HIGH);

    // ディスプレイレジスタをすべて0クリア
    digitalWrite(g_tm1638.stb_pin, LOW);
    for(i = 0; i < 16; i++)
    {
        shitf_out(0);
    }
    digitalWrite(g_tm1638.stb_pin, HIGH);

    // 7セグ表示
    digitalWrite(g_tm1638.stb_pin, LOW);
    shitf_out(TM1638_CMD_ADDR_BASE);
    for(i = 0; i < 8; i++)
    {
        shitf_out(0x7F);
        shitf_out(0);
    }
    digitalWrite(g_tm1638.stb_pin, HIGH);
#endif
    // Serial.printf("[DEBUG]TM1638 Drv Test\n");
}

void loop()
{
#if 0
    uint8_t i;

    for (i = 0; i < g_tm1638.seg_cnt; i++)
    {
        // Serial.printf("[DEBUG]TM1638 POS%d = %d\n", i, 0);
        tm1638_send_7seg_data(i, 0);
    }

    delay(1000);
#endif
}