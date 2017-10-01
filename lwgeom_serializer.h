/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lwgeom_serializer.h
 * Author: fzn
 *
 * Created on 2017年8月27日, 下午9:21
 */

#ifndef LWGEOM_SERIALIZER_H
#define LWGEOM_SERIALIZER_H

#include <impala_udf/udf.h>

extern "C" {
#include <liblwgeom.h>
}

using namespace impala_udf;

#ifdef __cplusplus
extern "C" {
#endif

    LWGEOM* HexWKBToLWGEOM(const StringVal& hexwkb);

    LWGEOM* WKTToLWGEOM(FunctionContext* context, const StringVal& wktText);
    
    StringVal LWGEOMToEWKT(FunctionContext* context, const LWGEOM* lwgeom);

#ifdef __cplusplus
}
#endif

#endif /* LWGEOM_SERIALIZER_H */

