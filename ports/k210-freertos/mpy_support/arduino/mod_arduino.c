/**
 * The MIT License (MIT)
 * 
 * Author: Baozhu Zuo (baozhu.zuo@gmail.com)
 * 
 * Copyright (C) 2019  Seeed Technology Co.,Ltd. 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"
#include <stdio.h>

#include "mod_arduino.h"
#include "arduino_wraper.h"
#include "Arduino.h"

#if MICROPY_PY_ARDUINO

//这是我们定义函数
STATIC mp_obj_t modtest_test0()
{
    pinMode(13,OUTPUT);
    serial_struct *serial;
    arduino_serial_new(&serial);
    arduino_serial_begin(serial,115200,2,6,7);
    arduino_serial_delete(serial);

    return mp_const_none;//不需要返回数据就返回它
}
//每一个我们和python接口的函数都需要使用这个宏定义
STATIC const MP_DEFINE_CONST_FUN_OBJ_0(modtest_obj_test0,modtest_test0);

STATIC const mp_map_elem_t arduino_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_arduino) },
    {MP_OBJ_NEW_QSTR(MP_QSTR_test0), MP_ROM_PTR(&modtest_obj_test0)},
};

STATIC MP_DEFINE_CONST_DICT (
    arduino_module_globals,
    arduino_module_globals_table
);

const mp_obj_module_t arduino_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&arduino_module_globals,
};

#endif // MICROPY_PY_ARDUINO
