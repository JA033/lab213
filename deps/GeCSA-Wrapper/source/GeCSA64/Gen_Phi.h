/*============================================
# Filename: Gen_Phi.h
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
  Compression structure of array Φ.
  we compress the array by saving the gap between Φ[i+1] and Φ[i],and four
different coding methods are used. the specific methods are as
follows:allone,gamma,bv,rldelta 说明： 数组Φ的压缩结构 我们通过存储 Φ[i+1] 和
Φ[i]的差值来压缩数组 Φ，在这里我们使用了四种不同的压缩方式
  具体为：allone,gamma,bv,rldelta
=============================================*/
#ifndef _GBV_H_
#define _GBV_H_
#include "ABS_Phi.h"
#include "BaseClass.h"
#include "InArray.h"
#include "broadword.hpp"
#include "parmaters.h"
#include "select.h"
#include <cstring>

class GEN_Phi : public ABS_Phi {
public:
    GEN_Phi(parmaters *csa, i64 *phivalue, double multi = 1.0);
    GEN_Phi();
    ~GEN_Phi();
    i64 GetValue(i64 i);
    i64 RightBoundary(i64 r, i64 L, i64 R); // count时左区间
    i64 LeftBoundary(i64 l, i64 L, i64 R);  // count时右区间
    i64 GetMemorySize();
    integer write(savekit &s);
    integer load(loadkit &s);
    void Testforgetphivalue();

    i64 bvsum;
    i64 gamasum;
    i64 rldatasum;
    i64 sizeofEcode(int type);

private:
    i64 mod(i64 x);
    i64 GetOneBit(i64 i);
    integer a;
    integer b;
    integer alpsize;
    integer tablewidth;
    u16 *zerostable_gamma;
    u16 *decodevaluenum_gamma;
    u16 *decodebitsnum_gamma;
    u16 *decoderesult_gamma;

    u16 *zerostable;
    u16 *decodevaluenum;
    u16 *decodebitsnum;
    u16 *decoderesult;

    i64 *superoffset;
    InArray *samples;
    InArray *offsects;
    InArray *method;
    u32 *sequence;
    i64 index;
    i64 lenofsequence;
    i64 lenofsuperoffset;
    i64 n;
    i64 *phivalue;

    enum CodeType { gamma, bv, allone, rldelta };

    double multi;

    i64 blogsize(i64 x);
    void SamplingAndCoding(parmaters *csa); // 压缩phi的主要函数
    void Test();
    void Append_Gamma(i64 x);
    i64 Decodegamma(i64 &position, i64 &x);
    i64 Decodegamma(i64 &postion, i64 &x, i64 zero);
    i64 ZerosRun_Gamma(i64 &position);
    u64 GetBits_Gamma(i64 position, i64 x);
    i64 GetValue(i64 i, i64 &position);

    i64 deltasize(i64 v);
    void Append(i64 x);
    i64 DecodeDelta(i64 &position, i64 &x);
    i64 DecodeDelta2(i64 &position, i64 &x);
    i64 DecodeDeltaBlock(i64 position, i64 base, i64 num);
    u64 GetBits(i64 position, i64 x);
    i64 ZerosRun(i64 &position);
};

#endif