/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   geography_inout.h
 * Author: fzn
 *
 * Created on 2017年8月6日, 下午11:09
 */

#ifndef GEOGRAPHY_INOUT_H
#define GEOGRAPHY_INOUT_H

#include <impala_udf/udf.h>

using namespace impala_udf;

#ifdef __cplusplus
extern "C" {
#endif

    
    /**
     * From WKT to bytes string of serialized geography object
     * @param context UDF context object
     * @param wktText WKT
     * @return Bytes string of serialized geography object
     */
    StringVal geography_from_text(FunctionContext* context, const StringVal& wktText);
    

#ifdef __cplusplus
}
#endif

#endif /* GEOGRAPHY_INOUT_H */

