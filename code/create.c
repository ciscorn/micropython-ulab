/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 * Copyright (c) 2019-2020 Zoltán Vörös
*/


#include "py/obj.h"
#include "py/runtime.h"
#include "create.h"

mp_obj_t create_zeros_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, uint8_t kind) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} } ,
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    uint8_t dtype = args[1].u_int;
    if(!MP_OBJ_IS_INT(args[0].u_obj) && !MP_OBJ_IS_TYPE(args[0].u_obj, &mp_type_tuple)) {
        mp_raise_TypeError(translate("input argument must be an integer or a 2-tuple"));
    }
    ndarray_obj_t *ndarray = NULL;
    if(MP_OBJ_IS_INT(args[0].u_obj)) {
        size_t n = mp_obj_get_int(args[0].u_obj);
        ndarray = ndarray_new_linear_array(n, dtype);
    } else if(MP_OBJ_IS_TYPE(args[0].u_obj, &mp_type_tuple)) {
        mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(args[0].u_obj);
        if(tuple->len != 2) {
            mp_raise_TypeError(translate("input argument must be an integer or a 2-tuple"));
        }
        ndarray = ndarray_new_ndarray_from_tuple(tuple, dtype);
    }
    if(kind == 1) {
        mp_obj_t one = mp_obj_new_int(1);
        for(size_t i=0; i < ndarray->len; i++) {
            mp_binary_set_val_array(dtype, ndarray->array, i, one);
        }
    }
    return MP_OBJ_FROM_PTR(ndarray);
}

mp_obj_t create_zeros(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return create_zeros_ones(n_args, pos_args, kw_args, 0);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_zeros_obj, 0, create_zeros);

mp_obj_t create_ones(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return create_zeros_ones(n_args, pos_args, kw_args, 1);
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_ones_obj, 0, create_ones);

mp_obj_t create_linspace(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = mp_const_none } },
        { MP_QSTR_num, MP_ARG_INT, {.u_int = 50} },
        { MP_QSTR_endpoint, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_true} },
        { MP_QSTR_retstep, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = mp_const_false} },
        { MP_QSTR_dtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = NDARRAY_FLOAT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(2, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint16_t len = args[2].u_int;
    if(len < 2) {
        mp_raise_ValueError(translate("number of points must be at least 2"));
    }
    mp_float_t value, step;
    value = mp_obj_get_float(args[0].u_obj);
    uint8_t dtype = args[5].u_int;
    if(args[3].u_obj == mp_const_true) step = (mp_obj_get_float(args[1].u_obj)-value)/(len-1);
    else step = (mp_obj_get_float(args[1].u_obj)-value)/len;
    ndarray_obj_t *ndarray = ndarray_new_linear_array(len, dtype);
    if(dtype == NDARRAY_UINT8) {
        uint8_t *array = (uint8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (uint8_t)value;
    } else if(dtype == NDARRAY_INT8) {
        int8_t *array = (int8_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (int8_t)value;
    } else if(dtype == NDARRAY_UINT16) {
        uint16_t *array = (uint16_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (uint16_t)value;
    } else if(dtype == NDARRAY_INT16) {
        int16_t *array = (int16_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = (int16_t)value;
    } else {
        mp_float_t *array = (mp_float_t *)ndarray->array;
        for(size_t i=0; i < len; i++, value += step) array[i] = value;
    }
    if(args[4].u_obj == mp_const_false) {
        return MP_OBJ_FROM_PTR(ndarray);
    } else {
        mp_obj_t tuple[2];
        tuple[0] = ndarray;
        tuple[1] = mp_obj_new_float(step);
        return mp_obj_new_tuple(2, tuple);
    }
}

MP_DEFINE_CONST_FUN_OBJ_KW(create_linspace_obj, 2, create_linspace);