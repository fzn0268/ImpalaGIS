/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lwgeom_ig.h
 * Author: fanzhongning
 *
 * Created on 2017年9月3日, 下午9:54
 */

#ifndef LWGEOM_IG_H
#define LWGEOM_IG_H

extern "C" {
#include <liblwgeom.h>
}

#include <impala_udf/udf.h>

using namespace impala_udf;

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Standard macro for reporting parser errors to PostgreSQL
     */

    extern void ig_parser_errhint(FunctionContext* context, LWGEOM_PARSER_RESULT *lwg_parser_result);
    extern void ig_unparser_errhint(FunctionContext* context, LWGEOM_UNPARSER_RESULT *lwg_unparser_result);

    #define IG_PARSER_ERROR(context, lwg_parser_result) \
        do { \
                ig_parser_errhint(context, &lwg_parser_result); \
        } while(0);

    /*
     * Standard macro for reporting unparser errors to PostgreSQL
     */
    #define IG_UNPARSER_ERROR(context, lwg_unparser_result) \
        do { \
                ig_unparser_errhint(context, &lwg_unparser_result); \
        } while(0);


#ifdef __cplusplus
}
#endif

#endif /* LWGEOM_IG_H */

