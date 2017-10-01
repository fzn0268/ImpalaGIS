/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lwgeom_ogc.h
 * Author: fzn
 *
 * Created on 2017年8月27日, 下午9:55
 */

#ifndef LWGEOM_OGC_H
#define LWGEOM_OGC_H

#include <impala_udf/udf.h>

using namespace impala_udf;

#ifdef __cplusplus
extern "C" {
#endif

    
    /**
     * From bytes string of serialized geography object to WKT
     * @param context UDF context object
     * @param strGser Bytes string of serialized geography object
     * @return WKT
     */
    StringVal LWGEOM_asText(FunctionContext* context, const StringVal& strGser);
    

#ifdef __cplusplus
}
#endif

#endif /* LWGEOM_OGC_H */

