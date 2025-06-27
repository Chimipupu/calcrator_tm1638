/**
 * @file drv_tm1638.cpp
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief TM1638ドライバ
 * @version 0.1
 * @date 2025-06-27
 * 
 * @copyright Copyright (c) 2025 Chimipupu(https://github.com/Chimipupu)
 * 
 */

#include "drv_tm1638.hpp"

static tm1638_t s_tm1638;

static void send_cmd(uint8_t cmd);
static void shitf_out_byte_data(uint8_t data);
static uint8_t shitf_in_byte_data(void);

static void shitf_out_byte_data(uint8_t data)
{
    shiftOut(s_tm1638.dio_pin, s_tm1638.clk_pin, LSBFIRST, data);
}

static uint8_t shitf_in_byte_data(void)
{
    uint8_t read_val;

    read_val = shiftIn(s_tm1638.dio_pin, s_tm1638.clk_pin, LSBFIRST);

    return read_val;
}

static void send_cmd(uint8_t cmd)
{
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(cmd);
    digitalWrite(s_tm1638.stb_pin, HIGH);
}

/**
 * @brief TM1638初期化
 * 
 * @param p_tm1638 TM1638初期化構造体のポインタ
 */
void tm1638_init(tm1638_t tm1638)
{
    uint8_t i;

    s_tm1638 = tm1638;

#if 1
    // GPIO初期化
    pinMode(s_tm1638.stb_pin, OUTPUT);
    pinMode(s_tm1638.clk_pin, OUTPUT);
    pinMode(s_tm1638.dio_pin, OUTPUT);
    digitalWrite(s_tm1638.stb_pin, HIGH);
    digitalWrite(s_tm1638.clk_pin, HIGH);
    digitalWrite(s_tm1638.dio_pin, HIGH);
#endif

    // TM1638にディスプレイ有効を設定
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_WRITE_DISPLAY_REG);
    digitalWrite(s_tm1638.stb_pin, HIGH);

    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_DISPLAY_ON | 7);
    digitalWrite(s_tm1638.stb_pin, HIGH);

    // ディスプレイレジスタをすべて0クリア
    digitalWrite(s_tm1638.stb_pin, LOW);
    for(i = 0; i < 16; i++)
    {
        shitf_out_byte_data(0);
    }
    digitalWrite(s_tm1638.stb_pin, HIGH);

    // 7セグ表示
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_ADDR_BASE);
    for(i = 0; i < 8; i++)
    {
        shitf_out_byte_data(SEG_LED_BIT_ALL);
        shitf_out_byte_data(0);
        // shitf_out_byte_data(i);
    }
    digitalWrite(s_tm1638.stb_pin, HIGH);
}

/**
 * @brief 7セグLEDの数字データ送信
 * 
 * @param pos 何桁目の7セグか
 * @param val 7セグのビットパターンデータ
 */
void tm1638_send_7seg_data(uint8_t pos, uint8_t val)
{
    uint8_t seg_addr;

    seg_addr = (TM1638_CMD_ADDR_BASE | pos);
    send_cmd(TM1638_CMD_FIXED_ADDR);
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(seg_addr);
    shitf_out_byte_data(val);
    digitalWrite(s_tm1638.stb_pin, HIGH);
}

/**
 * @brief キースキャンレジスタの読み出し
 * 
 * @return uint8_t レジスタ値
 */
uint8_t tm1638_read_key_register(void)
{
    uint8_t i, data,key_reg;

    digitalWrite(s_tm1638.stb_pin, LOW);

    shitf_out_byte_data(READ_KEY_REGISTER);
    pinMode(s_tm1638.dio_pin, INPUT);
    delayMicroseconds(2); // 仕様では最小2usec
    data = shitf_in_byte_data();
    digitalWrite(s_tm1638.stb_pin, HIGH);
    pinMode(s_tm1638.dio_pin, OUTPUT);

    return data;
}