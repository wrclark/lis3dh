#ifndef REGISTERS_H
#define REGISTERS_H

#define REG_STATUS_REG_AUX 0x07 /* ro */
#define REG_OUT_ADC1_L     0x08 /* ro */
#define REG_OUT_ADC1_H     0x09 /* ro */
#define REG_OUT_ADC2_L     0x0A /* ro */
#define REG_OUT_ADC2_H     0x0B /* ro */
#define REG_OUT_ADC3_L     0x0C /* ro */
#define REG_OUT_ADC3_H     0x0D /* ro */
#define REG_WHO_AM_I       0x0F /* ro; default=0x33 */
#define REG_CTRL_REG0      0x1E /* rw; default=0x10 */
#define REG_TEMP_CFG_REG   0x1F /* rw; default=0x00 */
#define REG_CTRL_REG1      0x20 /* rw; default=0x07 */
#define REG_CTRL_REG2      0x21 /* rw; default=0x00 */
#define REG_CTRL_REG3      0x22 /* rw; default=0x00 */
#define REG_CTRL_REG4      0x23 /* rw; default=0x00 */
#define REG_CTRL_REG5      0x24 /* rw; default=0x00 */
#define REG_CTRL_REG6      0x25 /* rw; default=0x00 */
#define REG_REFERENCE      0x26 /* rw; default=0x00 */
#define REG_STATUS_REG     0x27 /* ro */
#define REG_OUT_X_L        0x28 /* ro */
#define REG_OUT_X_H        0x29 /* ro */
#define REG_OUT_Y_L        0x2A /* ro */
#define REG_OUT_Y_H        0x2B /* ro */
#define REG_OUT_Z_L        0x2C /* ro */
#define REG_OUT_Z_H        0x2D /* ro */
#define REG_FIFO_CTRL_REG  0x2E /* rw; default=0x00 */
#define REG_FIFO_SRC_REG   0x2F /* ro */
#define REG_INT1_CFG       0x30 /* rw; default=0x00 */
#define REG_INT1_SRC       0x31 /* ro */
#define REG_INT1_THS       0x32 /* rw; default=0x00 */
#define REG_INT1_DURATION  0x33 /* rw; default=0x00 */
#define REG_INT2_CFG       0x34 /* rw; default=0x00 */
#define REG_INT2_SRC       0x35 /* ro */
#define REG_INT2_THS       0x36 /* rw; default=0x00 */
#define REG_INT2_DURATION  0x37 /* rw; default=0x00 */
#define REG_CLICK_CFG      0x38 /* rw; default=0x00 */
#define REG_CLICK_SRC      0x39 /* ro */
#define REG_CLICK_THS      0x3A /* rw; default=0x00 */
#define REG_TIME_LIMIT     0x3B /* rw; default=0x00 */
#define REG_TIME_LATENCY   0x3C /* rw; default=0x00 */
#define REG_TIME_WINDOW    0x3D /* rw; default=0x00 */
#define REG_ACT_THS        0x3E /* rw; default=0x00 */
#define REG_ACT_DUR        0x3F /* rw; default=0x00 */


#endif