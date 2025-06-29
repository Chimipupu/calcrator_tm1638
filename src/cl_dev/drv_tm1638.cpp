#include <sys/_stdint.h>
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
static void shitf_out_byte_data(uint8_t data, bit_order bitOrder);
static uint8_t shitf_in_byte_data(bit_order bitOrder);
static void seg_update_auto_inc_addr(uint8_t *p_data_buf);
static void seg_update_fixed_addr(uint8_t *p_data_buf);
static void set_seg_bits(uint8_t digit, uint8_t seg_pattern);
static void clear_seg_bits(uint8_t digit);
static void seg_init(void);

static uint8_t s_seg_data_buf[DISPLAY_REG_BYTE] = {0};
const uint8_t G_SEG_NUM_DATA_BUF[9] = {SEG_LED_0, SEG_LED_1, SEG_LED_2, SEG_LED_3, SEG_LED_4, SEG_LED_5, SEG_LED_6, SEG_LED_7, SEG_LED_9};

# if 0
uint8_t swap_byte(uint8_t data)
{
    uint8_t swap = 0;
    swap = (data << 4) | (data >> 4);
    return swap;
}
#endif

/**
 * @brief 7セグの表示を初期化
 * 
 */
static void seg_init(void)
{
    set_seg_bits(SEG_DIGIT_1, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_2, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_3, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_4, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_5, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_6, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_7, SEG_LED_BLANK);
    set_seg_bits(SEG_DIGIT_8, SEG_LED_BLANK);

    seg_update_auto_inc_addr(&s_seg_data_buf[0]);
}

/**
 * @brief 指定の7セグ桁のビットをクリア
 * @param digit 7セグの桁（0〜7）
 */
static void clear_seg_bits(uint8_t digit)
{
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        uint8_t index = bit * 2;  // 偶数アドレス
        s_seg_data_buf[index] &= ~(1 << digit);
    }
}

/**
 * @brief 指定の7セグ桁のビットを設定
 * 
 * @param digit 7セグの桁（0〜7）
 * @param seg_pattern 7セグのビットパターン
 */
static void set_seg_bits(uint8_t digit, uint8_t seg_pattern)
{
    clear_seg_bits(digit);

    for (uint8_t bit = 0; bit < 8; bit++)
    {
        if (seg_pattern & (1 << bit)) {
            uint8_t index = bit * 2;  // 偶数アドレス
            s_seg_data_buf[index] |= (1 << digit);
        }
    }
}

static void shitf_out_byte_data(uint8_t data, bit_order bitOrder)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++)
    {
        if (bitOrder == LSB_FIRST) {
            digitalWrite(s_tm1638.dio_pin, !!(data & (1 << i)) ? HIGH : LOW);
        } else {
            digitalWrite(s_tm1638.dio_pin, !!(data & (1 << (7 - i))) ? HIGH : LOW);
        }

        digitalWrite(s_tm1638.clk_pin, HIGH);
        digitalWrite(s_tm1638.clk_pin, LOW);
    }
}

static uint8_t shitf_in_byte_data(bit_order bitOrder)
{
    uint8_t i, read_val = 0;

    for (i = 0; i < 8; ++i)
    {
        digitalWrite(s_tm1638.clk_pin, HIGH);
        if (bitOrder == LSB_FIRST) {
            read_val |= digitalRead(s_tm1638.dio_pin) << i;
        } else {
            read_val |= digitalRead(s_tm1638.dio_pin) << (7 - i);
        }
        digitalWrite(s_tm1638.clk_pin, LOW);
    }

    return read_val;
}

static void send_cmd(uint8_t cmd)
{
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(cmd, LSB_FIRST);
    digitalWrite(s_tm1638.stb_pin, HIGH);
}

/**
 * @brief 自動インクリメントでのディスプレイレジスタ更新
 * 
 * @param p_data_buf 7セグの表示データ
 */
static void seg_update_auto_inc_addr(uint8_t *p_data_buf)
{
    uint8_t i;

    // 自動インクリメントコマンド
    send_cmd(TM1638_CMD_WRITE_DISPLAY_REG);

    // ディスプレイレジスタを初期化
    digitalWrite(s_tm1638.stb_pin, LOW);
    shitf_out_byte_data(TM1638_CMD_ADDR_BASE, LSB_FIRST);
    for(i = 0; i < DISPLAY_REG_BYTE; i++)
    {
        shitf_out_byte_data(*p_data_buf, LSB_FIRST);
        p_data_buf++;
    }
    digitalWrite(s_tm1638.stb_pin, HIGH);

    // 輝度の初期化
    send_cmd(TM1638_CMD_DISPLAY_MAX_BRIGHTNESS);
}

/**
 * @brief アドレス指定でのディスプレイレジスタ更新
 * 
 * @param p_data_buf 7セグの表示データ
 */
 static void seg_update_fixed_addr(uint8_t *p_data_buf)
{
    uint8_t i;

    // アドレス指定コマンド
    send_cmd(TM1638_CMD_FIXED_ADDR);

    // ディスプレイレジスタを初期化
    for(i = 0; i < DISPLAY_REG_BYTE; i++)
    {
        digitalWrite(s_tm1638.stb_pin, LOW);
        shitf_out_byte_data(TM1638_CMD_ADDR_BASE | i, LSB_FIRST);
        shitf_out_byte_data(*p_data_buf, LSB_FIRST);
        p_data_buf++;
        digitalWrite(s_tm1638.stb_pin, HIGH);
    }

    // 輝度の初期化
    send_cmd(TM1638_CMD_DISPLAY_MAX_BRIGHTNESS);
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

    // 7セグの表示を初期化
    seg_init();
}

/**
 * @brief uint32の値を8桁の7セグに表示
 * @param val 表示するuint32の値
 */
void tm1638_uint32_to_7seg(uint32_t val)
{
    uint8_t digit = 0;
    uint8_t seg_pattern = SEG_LED_BLANK;

    // 8桁で表示できる最大の整数か?
    if (val >= SEG_U32_MAX) {
        return;
    } else {
        for (int i = 0; i < 8; i++)
        {
            digit = val % 10;
            val /= 10;

            switch (digit) {
                case 0:
                    seg_pattern = SEG_LED_0;
                    break;

                case 1:
                    seg_pattern = SEG_LED_1;
                    break;

                case 2:
                    seg_pattern = SEG_LED_2;
                    break;

                case 3:
                    seg_pattern = SEG_LED_3;
                    break;

                case 4:
                    seg_pattern = SEG_LED_4;
                    break;

                case 5:
                    seg_pattern = SEG_LED_5;
                    break;

                case 6:
                    seg_pattern = SEG_LED_6;
                    break;

                case 7:
                    seg_pattern = SEG_LED_7;
                    break;

                case 8:
                    seg_pattern = SEG_LED_8;
                    break;

                case 9:
                    seg_pattern = SEG_LED_9;
                    break;

                default:
                    seg_pattern = SEG_LED_BLANK;
                    break;
            }
            set_seg_bits(i, seg_pattern);
            seg_update_fixed_addr(&s_seg_data_buf[0]);
        }
    }
}

/**
 * @brief floatの値を8桁の7セグに表示
 * @param val 表示するfloatの値（例: 1234.567）
 * 小数点を含めて8桁以内に収める必要がある。
 */
void tm1638_float_to_7seg(float val)
{
    char str[32] = {0};
    uint8_t i, seg_pattern;
    uint8_t seg_index = 0;

    memset(&str[0], 0, sizeof(str));
    snprintf(str, sizeof(str), "%f", val);

    for (i = 7; ((str[i] != '\0') || seg_index < 8); i--)
    {
        switch (str[i]) {
            case '0':
                seg_pattern = SEG_LED_0;
                break;
            case '1':
                seg_pattern = SEG_LED_1;
                break;
            case '2':
                seg_pattern = SEG_LED_2;
                break;
            case '3':
                seg_pattern = SEG_LED_3;
                break;
            case '4':
                seg_pattern = SEG_LED_4;
                break;
            case '5':
                seg_pattern = SEG_LED_5;
                break;
            case '6':
                seg_pattern = SEG_LED_6;
                break;
            case '7':
                seg_pattern = SEG_LED_7;
                break;
            case '8':
                seg_pattern = SEG_LED_8;
                break;
            case '9':
                seg_pattern = SEG_LED_9;
                break;
            case '/':
                seg_pattern = SEG_LED_9;
                break;
            case '.':
                seg_pattern = SEG_LED_DP;
                break;
            case ' ':
            default:
                seg_pattern = SEG_LED_BLANK;
                break;
        }

        set_seg_bits(seg_index, seg_pattern);
        seg_update_fixed_addr(&s_seg_data_buf[0]);
        seg_index++;
    }
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
    send_cmd(READ_KEY_REGISTER);
    delayMicroseconds(2); // 仕様では最小2usec
    // 4Byteキースキャン
    for (i = 0; i < KEY_REG_BYTE; i++)
    {
        byte[i] = shitf_in_byte_data(LSB_FIRST);
        // Serial.printf("[DEBUG]TM1638 Key Reg%d = 0x%02\n", i, byte[i]);
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