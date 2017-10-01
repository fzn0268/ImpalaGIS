/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lwgeom_functions_basic.h
 * Author: fzn
 *
 * Created on 2017年8月27日, 下午10:02
 */

#ifndef LWGEOM_FUNCTIONS_BASIC_H
#define LWGEOM_FUNCTIONS_BASIC_H

#include <impala_udf/udf.h>

using namespace impala_udf;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * From Bytes string of serialized geography object to WKT to EWKT
     * @param context UDF context object
     * @param hexwkb
     * @return EWKT
     */
    StringVal LWGEOM_asEWKT(FunctionContext* context, const StringVal& hexwkb);
    
    /**
     * 
     * @param context
     * @param wktText
     * @return summary info on a GEOGRAPHY
     */
    StringVal LWGEOM_summary(FunctionContext* context, const StringVal& wktText);


#ifdef __cplusplus
}
#endif

#endif /* LWGEOM_FUNCTIONS_BASIC_H */

