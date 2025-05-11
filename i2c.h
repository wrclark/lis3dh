/*
 * This file is part of lis3dh.
 *
 * Copyright (C) 2025 William Clark
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef I2C_H
#define I2C_H

#include <stdint.h>

int i2c_init (void);
int i2c_read (uint8_t reg, uint8_t *dst, uint32_t size);
int i2c_write (uint8_t reg, uint8_t value);
int i2c_deinit (void);

#endif