// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/*============================================
# Filename: InArray.h
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
  int vector,every ceil needs data_width bits.
  it also integrates a rank / select compression structure.
  说明：
  整形向量，每个元素需要data_width比特
  在这个类中也集成了一个rank/select压缩结构
=============================================*/
#ifndef _Inarray
#define _Inarray
#include "BaseClass.h"
#include "loadkit.h"
#include "savekit.h"
class InArray {
public:
    InArray();
    InArray(u64 data_num, u32 data_width);
    ~InArray(void);

    u64 GetValue(u64 index);
    void SetValue(u64 index, u64 value);
    void constructrank(int Blength, int SBlength);
    void constructrank(u64 data_num, u64 *positions, u64 num, int Blength, int SBlength);
    
    u64 GetNum();
    u32 GetDataWidth();
    u64 GetMemorySize();
    u64 GetValue2(u64 index);
    u64 GetrankMemorySize();
    integer getD(u64 i);
    u64 rank(u64 i);
    u64 rankGeneral(u64 i);
    u64 select(u64 i);
    u64 blockbinarySearch(InArray *B, u64 k, u64 l, u64 h);
    u32 *rawdata() { return data; };

    i64 write(savekit &s);
    i64 load(loadkit &s, int flag);

private:
    u32 *data;
    u64 datanum;
    u32 datawidth;
    u64 mask;
    int rankflag;
    InArray *Md;
    InArray *Sd;
    InArray *SBd;
    InArray *Bd;
    int Blength;
    int SBlength;
    i64 block_len;
    i64 super_block_len;
};
#endif
