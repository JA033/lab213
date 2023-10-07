/*============================================
# Filename: ABS_Phi.h
# Ver 2.0 2020-10-10
# Copyright (C) 2020 PengLong HongweiHuo
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the
License.
#
# Description:
 the abstract class of Phi
 include the interface whitch support to class CSA
  说明：
  这是关于Phi类的抽象类
  包含了需要提供给CSA类的接口
=============================================*/
#ifndef _ABS_PHI
#define _ABS_PHI
#include "BaseClass.h"
#include "loadkit.h"
#include "savekit.h"
class ABS_Phi {
public:
    virtual ~ABS_Phi() {};
    // virtual integer * GetPhiArray()=0;
    // virtual void GetSubPhiArray(integer,integer,integer *)=0;
    virtual i64 GetValue(i64 i) = 0;
    virtual i64 RightBoundary(i64 r, i64 L, i64 R) = 0;
    virtual i64 LeftBoundary(i64 l, i64 L, i64 R) = 0;
    virtual i64 GetMemorySize() = 0;
    virtual integer write(savekit &s) = 0;
    virtual integer load(loadkit &s) = 0;
    virtual i64 sizeofEcode(int type) = 0;
};
#endif
