// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/*============================================
# Filename: Phi.h
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
  A proxy class of the kinds of Phi classes.
  so it holds a base-class pointer of Gen_Phi object and provide appropriate
data types to Gen_Phi object. 说明： Phi类的代理类
  这个类中包含了Gen_Phi对象的基类指针，并提供适用于Gen_Phi对象的数据类型
=============================================*/
#include "ABS_Phi.h"
#include "BaseClass.h"
#include "Gen_Phi.h"
#include "loadkit.h"
#include "parmaters.h"
#include "savekit.h"
#include <map>
class Phi {
public:
    Phi(parmaters *csa);
    Phi();
    ~Phi();

    // integer *GetPhiArray();
    // void GetPhiPiece(integer index,integer num,integer *phipiece);
    i64 GetValue(i64 i);
    i64 RightBoundary(i64 r, i64 L, i64 R);
    i64 LeftBoundary(i64 l, i64 L, i64 R);
    i64 Size();

    integer write(savekit &s);
    integer load(loadkit &s);
    i64 **doublec;
    i64 *range;
    i64 mini;
    int range_len;

    ABS_Phi *getImpl() { return phi; }

    i64 getEcodeSpace(int type);

    u64 PrimayIndex;

private:
    i64 *PreSpace(parmaters *csa);
    ABS_Phi *phi;
    uchar *T;
    i64 *code;
    i64 *start;
    i64 n;
    void randchar(uchar *T);
    i64 left(i64 i, i64 j);
    i64 right(i64 i, i64 j);
    i64 *phivalue;
    integer type;
    i64 blogsize(i64 x);
    i64 puredelta(i64 x);
    uchar lastchar;
};
