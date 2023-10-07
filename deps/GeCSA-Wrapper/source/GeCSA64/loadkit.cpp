// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/*============================================
# Filename: loadkit.cpp
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
=============================================*/
#include "loadkit.h"
void loadkit::close() {
    if (r != NULL)
        fclose(r);
    r = NULL;
}

loadkit::~loadkit() {
    if (r != NULL)
        fclose(r);
}

loadkit::loadkit(const char *file) {
    this->r = fopen(file, "rb");
    if (r == NULL) {
        cout << "Fopen error" << endl;
        exit(0);
    }
}

integer loadkit::loadi64(i64 &value) {
    integer num = fread(&value, sizeof(i64), 1, r);
    return num;
}

integer loadkit::loadu64(u64 &value) {
    integer num = fread(&value, sizeof(u64), 1, r);
    return num;
}

integer loadkit::loadinteger(integer &value) {
    integer num = fread(&value, sizeof(integer), 1, r);
    return num;
}

integer loadkit::loadu32(u32 &value) {
    integer num = fread(&value, sizeof(u32), 1, r);
    return num;
}

integer loadkit::loadi16(i16 &value) {
    integer num = fread(&value, sizeof(i16), 1, r);
    return num;
}

integer loadkit::loadu16(u16 &value) {
    integer num = fread(&value, sizeof(u16), 1, r);
    return num;
}

integer loadkit::loadi64array(i64 *value, integer len) {
    i64 num = fread(value, sizeof(i64), len, r);
    return num;
}
i64 loadkit::loadi64array(i64 *value, i64 len) {
    i64 num = fread(value, sizeof(i64), len, r);
    return num;
}
integer loadkit::loadu64array(u64 *value, integer len) {
    integer num = fread(value, sizeof(u64), len, r);
    return num;
}

integer loadkit::loadintegerarray(integer *value, integer len) {
    integer num = fread(value, sizeof(integer), len, r);
    return num;
}

integer loadkit::loadu32array(u32 *value, integer len) {
    integer num = fread(value, sizeof(u32), len, r);
    return num;
}
i64 loadkit::loadu32array(u32 *value, i64 len) {
    i64 num = fread(value, sizeof(u32), len, r);
    return num;
}
integer loadkit::loadi16array(i16 *value, integer len) {
    integer num = fread(value, sizeof(i16), len, r);
    return num;
}

integer loadkit::loadu16array(u16 *value, integer len) {
    integer num = fread(value, sizeof(u16), len, r);
    return num;
}

void loadkit::skip(integer len) { fseek(r, len, SEEK_CUR); }
