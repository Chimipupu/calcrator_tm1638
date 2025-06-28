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
static void auto_inc_addr_mode_init(void);
static void fix_addr_mode_init(void);

static uint8_t s_seg_data_buf[DISPLAY_REG_BYTE] = {0};

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
 * @brief 自動インクリメントでの初期化処理
 * TM1638のデータシートの1つ目のフローチャート
 */
static void auto_inc_addr_mode_init(void)
{
    uint8_t i;

    // 自動インクリメントコマンド
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_WRITE_DISPLAY_REG);
    digitalWrite(s_tm1638.stb_pin, HIGH);

    // ディスプレイレジスタを初期化
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_ADDR_BASE);
    for(i = 0; i < DISPLAY_REG_BYTE; i++)
    {
        shitf_out_byte_data(s_seg_data_buf[i]);
    }
    digitalWrite(s_tm1638.stb_pin, HIGH);

    // 輝度の初期化
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_DISPLAY_MAX_BRIGHTNESS);
    digitalWrite(s_tm1638.stb_pin, HIGH);
}

/**
 * @brief アドレス指定での初期化処理
 * TM1638のデータシートの2つ目のフローチャート
 */
static void fix_addr_mode_init(void)
{
    uint8_t i;

    // 自動インクリメントコマンド
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_FIXED_ADDR);
    digitalWrite(s_tm1638.stb_pin, HIGH);

    // ディスプレイレジスタを初期化
    for(i = 0; i < DISPLAY_REG_BYTE; i++)
    {
        digitalWrite(s_tm1638.stb_pin, LOW);
        shitf_out_byte_data(TM1638_CMD_ADDR_BASE | i);
        shitf_out_byte_data(s_seg_data_buf[i]);
        digitalWrite(s_tm1638.stb_pin, HIGH);
    }

    // 輝度の初期化
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_DISPLAY_MAX_BRIGHTNESS);
    digitalWrite(s_tm1638.stb_pin, HIGH);
}

/**
 * @brief TM1638初期化
 * 
 * @param p_tm1638 TM1638初期化構造体のポインタ
 */
void tm1638_init(tm1638_t tm1638)
{
    s_tm1638 = tm1638;

    // GPIO初期化
    pinMode(s_tm1638.stb_pin, OUTPUT);
    pinMode(s_tm1638.clk_pin, OUTPUT);
    pinMode(s_tm1638.dio_pin, OUTPUT);
    digitalWrite(s_tm1638.stb_pin, HIGH);
    digitalWrite(s_tm1638.clk_pin, HIGH);
    digitalWrite(s_tm1638.dio_pin, HIGH);

    // 7セグデータの初期値を詰め込み
    s_seg_data_buf[0] = SEG_LED_8;  // SEG1~8のデータ
    s_seg_data_buf[1] = 0;          // SEG9,10データ
    s_seg_data_buf[2] = SEG_LED_7;
    s_seg_data_buf[3] = 0;
    s_seg_data_buf[4] = SEG_LED_6;
    s_seg_data_buf[5] = 0;
    s_seg_data_buf[6] = SEG_LED_5;
    s_seg_data_buf[7] = 0;
    s_seg_data_buf[8] = SEG_LED_4;
    s_seg_data_buf[9] = 0;
    s_seg_data_buf[10] = SEG_LED_3;
    s_seg_data_buf[11] = 0;
    s_seg_data_buf[12] = SEG_LED_2;
    s_seg_data_buf[13] = 0;
    s_seg_data_buf[14] = SEG_LED_1;
    s_seg_data_buf[15] = 0;

    #if 1
    auto_inc_addr_mode_init();
#else
    fix_addr_mode_init();
#endif
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
uint8_t tm1638_read_key(void)
{
    uint8_t i, key = 0;
    uint8_t byte[KEY_REG_BYTE] = {0};

    memset(&byte[0], 0x00, sizeof(byte));

    // キーレジスタ読み出し
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(READ_KEY_REGISTER);
    pinMode(s_tm1638.dio_pin, INPUT);
    delayMicroseconds(2); // 仕様では最小2usec
    // 4Byteキースキャン
    for (i = 0; i < KEY_REG_BYTE; i++)
    {
        byte[i] = shitf_in_byte_data();
        Serial.printf("[DEBUG]TM1638 Key Reg%d = 0x%02\n", i, byte[i]);
    }
    digitalWrite(s_tm1638.stb_pin, HIGH);
    pinMode(s_tm1638.dio_pin, OUTPUT);

    // キーの判定
    byte[0] = byte[0] & KEY_REG_KS1_KS2;
    byte[1] = byte[1] & KEY_REG_KS3_KS4;
    byte[2] = byte[2] & KEY_REG_KS5_KS6;
    byte[3] = byte[3] & KEY_REG_KS7_KS8;

    for (i = 0; i < KEY_REG_BYTE; i++)
    {
        if(byte[i] != 0){
            key = byte[i];
        }
    }

    return key;
}