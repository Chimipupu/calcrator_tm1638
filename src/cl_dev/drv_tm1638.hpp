/**
 * @file drv_tm1638.hpp
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief TM1638ドライバのヘッダー
 * @version 0.1
 * @date 2025-06-27
 * 
 * @copyright Copyright (c) 2025 Chimipupu(https://github.com/Chimipupu)
 * 
 */
#ifndef DRV_TM1638_HPP
#define DRV_TM1638_HPP

#include <stdint.h>
#include <string.h>
#include <Arduino.h>

// (DEBU)デバッグ有効マクロ
// #define TM1638_DRV_DEBUG
#ifdef TM1638_DRV_DEBUG
void dbg_7seg_test(void);
#endif // TM1638_DRV_DEBUG

typedef enum {
    LSB_FIRST = 0,
    MSB_FIRST = 1,
} bit_order;

// データ設定コマンド
#define TM1638_CMD_WRITE_DISPLAY_REG        0x40  // ディスプレイレジスタ書き込み
#define TM1638_CMD_READ_KEY_DATA            0x42  // キースキャンデータ読み出し
#define TM1638_CMD_AUTO_INC                 0x44  // 自動インクリメント
#define TM1638_CMD_FIXED_ADDR               0x44  // アドレス指定

// アドレス設定コマンド
#define TM1638_CMD_ADDR_BASE                0xC0  // 表示・LED用アドレス先頭（+0～0x0Fで指定）

// ディスプレイ制御コマンド
#define TM1638_CMD_DISPLAY                  0x80  // 表示制御コマンド先頭
#define TM1638_CMD_DISPLAY_OFF              0x80  // 7セグ非表示
#define TM1638_CMD_DISPLAY_ON               0x88  // 7セグ表示
#define TM1638_CMD_DISPLAY_MAX_BRIGHTNESS   0x8F  // 最大輝度
#define DISPLAY_REG_BYTE                    0x10

// キーレジスタ
#define READ_KEY_REGISTER                   0x42
#define KEY_REG_BYTE                        4

#if 1
// QYF-TM1638基板はK1,K2の2x8=16bitなので0x66でマスク
#define KEY_REG_KS1_KS2                     0x66
#define KEY_REG_KS3_KS4                     0x66
#define KEY_REG_KS5_KS6                     0x66
#define KEY_REG_KS7_KS8                     0x66
#else
// 基板がK1,K2,K3の3x8=24bitなら0xEEでマスク
#define KEY_REG_KS1_KS2                     0xEE
#define KEY_REG_KS3_KS4                     0xEE
#define KEY_REG_KS5_KS6                     0xEE
#define KEY_REG_KS7_KS8                     0xEE
#endif

// 7セグのビットパターン (MSB: dot, g, f, e, d, c, b, a)
#define SEG_LED_0                           0b00111111  // 0
#define SEG_LED_1                           0b00000110  // 1
#define SEG_LED_2                           0b01011011  // 2
#define SEG_LED_3                           0b01001111  // 3
#define SEG_LED_4                           0b01100110  // 4
#define SEG_LED_5                           0b01101101  // 5
#define SEG_LED_6                           0b01111101  // 6
#define SEG_LED_7                           0b00000111  // 7
#define SEG_LED_8                           0b01111111  // 8
#define SEG_LED_9                           0b01101111  // 9
#define SEG_LED_A                           0b01110111  // A
#define SEG_LED_B                           0b01111100  // b
#define SEG_LED_C                           0b00111001  // C
#define SEG_LED_D                           0b01011110  // d
#define SEG_LED_E                           0b01111001  // E
#define SEG_LED_F                           0b01110001  // F
#define SEG_LED_G                           0b00111101  // G
#define SEG_LED_H                           0b01110110  // H
#define SEG_LED_I                           0b00000110  // I
#define SEG_LED_J                           0b00011110  // J
#define SEG_LED_L                           0b00111000  // L
#define SEG_LED_N                           0b01010100  // n
#define SEG_LED_O                           0b00111111  // O
#define SEG_LED_P                           0b01110011  // P
#define SEG_LED_Q                           0b01100111  // q
#define SEG_LED_R                           0b01010000  // r
#define SEG_LED_S                           0b01101101  // S
#define SEG_LED_T                           0b01111000  // t
#define SEG_LED_U                           0b00111110  // U
#define SEG_LED_Y                           0b01101110  // y
#define SEG_LED_DASH                        0b01000000  // -
#define SEG_LED_UNDER                       0b00001000  // _
#define SEG_LED_EQUAL                       0b01001000  // =
#define SEG_LED_BLANK                       0b00000000  // 消灯
#define SEG_LED_BIT_ALL                     0b01111111  // 全点灯（DPを除く）
#define SEG_LED_DP                          0b10000000  // .（dot）

// 7セグの桁選択用（DIGIT 1～8）
#define SEG_DIGIT_1                         0           // 7セグ 1桁目
#define SEG_DIGIT_2                         1           // 7セグ 2桁目
#define SEG_DIGIT_3                         2           // 7セグ 3桁目
#define SEG_DIGIT_4                         3           // 7セグ 4桁目
#define SEG_DIGIT_5                         4           // 7セグ 5桁目
#define SEG_DIGIT_6                         5           // 7セグ 6桁目
#define SEG_DIGIT_7                         6           // 7セグ 7桁目
#define SEG_DIGIT_8                         7           // 7セグ 8桁目

// その他の7セグ関連
#define SEG_U32_MAX                         99999999    // 8桁で表示できる最大の整数

// TM1638構造体
typedef struct {
    uint8_t stb_pin;        // TM1638のSTBと接続しているGPIOピン
    uint8_t clk_pin;        // TM1638のCLKと接続しているGPIOピン
    uint8_t dio_pin;        // TM1638のDIOと接続しているGPIOピン
    uint8_t seg_cnt;        // 7セグLEDを数（MAX:8個）
    uint8_t key_bit_cnt;    // キースイッチの数（MAX:3x8）
} tm1638_t;

void tm1638_uint32_to_7seg(uint32_t val);
void tm1638_float_to_7seg(float val);
uint8_t tm1638_key_read(void);
void tm1638_init(tm1638_t tm1638);
void tm1638_update(void);

#endif // DRV_TM1638_HPP