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
//TODO:一次最大只能读
#include "stdint.h"
#include "stdio.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/binary.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/parsenum.h"
#include "py/formatfloat.h"
#include "py/runtime.h"
#include "lib/utils/interrupt_char.h"

#include "mod_arduino.h"

STATIC mp_obj_t mp_arduino_serial_begin(size_t n_args, const mp_obj_t *args) {
  
    arduino_serial_obj_t *self = (arduino_serial_obj_t *)(args[0]);
    printf("mp_arduino_serial_begin:%p\n",self->serial);
    if(n_args != 5){
        mp_raise_ValueError("serial begin neeed 4 args, (baudrate, uart_num, rx_pin, tx_pin");
    }
    else
        arduino_serial_begin(self->serial,
                        mp_obj_get_int(args[1]), //baudrate
                        mp_obj_get_int(args[2]), //uart_num
                        mp_obj_get_int(args[3]), //rx_pin
                        mp_obj_get_int(args[4])  //tx_pin
        );  
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arduino_serial_begin_obj, 5, 5, mp_arduino_serial_begin);

STATIC mp_obj_t mp_arduino_serial_end(mp_obj_t self_in) {
    arduino_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    arduino_serial_end(self->serial);
    
    m_del_obj(arduino_serial_obj_t, self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(arduino_serial_end_obj,mp_arduino_serial_end);

STATIC mp_obj_t mp_arduino_serial_available(mp_obj_t self_in) {
    arduino_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return  arduino_serial_available(self->serial);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(arduino_serial_available_obj,mp_arduino_serial_available);

STATIC mp_obj_t mp_arduino_serial_flush(mp_obj_t self_in) {
    arduino_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    arduino_serial_end(self->serial);
    return mp_const_none;    
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(arduino_serial_flush_obj,mp_arduino_serial_flush);



STATIC const mp_rom_map_elem_t arduino_serial_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_begin), MP_ROM_PTR(&arduino_serial_begin_obj) },
     { MP_ROM_QSTR(MP_QSTR__del__), MP_ROM_PTR(&arduino_serial_end_obj) },
    { MP_ROM_QSTR(MP_QSTR_end), MP_ROM_PTR(&arduino_serial_end_obj) },
    { MP_ROM_QSTR(MP_QSTR_available), MP_ROM_PTR(&arduino_serial_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&arduino_serial_flush_obj) },

    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};


STATIC MP_DEFINE_CONST_DICT(arduino_serial_locals_dict, arduino_serial_locals_dict_table);

STATIC mp_uint_t mp_arduino_serial_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    arduino_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return arduino_serial_read(self->serial,buf_in,size);
}

STATIC mp_uint_t mp_arduino_serial_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    arduino_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return arduino_serial_write(self->serial,buf_in,size);
}

STATIC mp_uint_t mp_arduino_serial_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    arduino_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && arduino_serial_available(self->serial)) {
            ret |= MP_STREAM_POLL_RD;
        }
		//TODO:add Judging transmission enable
        if ((flags & MP_STREAM_POLL_WR) ) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;    
}

STATIC mp_obj_t mp_arduino_serial_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, true); 
    // create instance
    arduino_serial_obj_t *self = m_new_obj(arduino_serial_obj_t);
    self->base.type = &arduino_serial_type;
    arduino_serial_new(&self->serial);
    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_stream_p_t arduino_serial_stream_p = {
    .read = mp_arduino_serial_read,
    .write = mp_arduino_serial_write,
    .ioctl = mp_arduino_serial_ioctl,
    .is_text = false,
};

const mp_obj_type_t arduino_serial_type = {
    .base={ &mp_type_type }, 
    .name = MP_QSTR_Serial, 
    .make_new = mp_arduino_serial_make_new,
    .protocol = &arduino_serial_stream_p,         
    .locals_dict = (mp_obj_dict_t*)&arduino_serial_locals_dict,   
};