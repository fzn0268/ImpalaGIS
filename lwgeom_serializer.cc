/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <float.h>

#include "lwgeom_ig.h"
#include "lwgeom_serializer.h"

LWGEOM* HexWKBToLWGEOM(const StringVal& hexwkb) {
    return lwgeom_from_hexwkb((char*) hexwkb.ptr, LW_PARSER_CHECK_NONE);
}

LWGEOM* WKTToLWGEOM(FunctionContext* context, const StringVal& wktText) {
    LWGEOM_PARSER_RESULT lwg_parser_result;
    
    /* Extract the cstring from the stringval TODO copy string */
    char *wkt = (char *) wktText.ptr;

    /* Pass the cstring to the input parser, and magic occurs! */
    if (lwgeom_parse_wkt(&lwg_parser_result, wkt, LW_PARSER_CHECK_ALL) == LW_FAILURE)
        IG_PARSER_ERROR(context, lwg_parser_result);
    
    return lwg_parser_result.geom;
}

StringVal LWGEOMToEWKT(FunctionContext* context, const LWGEOM* lwgeom) {
    char *wkt;
    size_t wkt_size;
    
    /* Write to WKT and free the geometry */
    wkt = lwgeom_to_wkt(lwgeom, WKT_EXTENDED, DBL_DIG, &wkt_size);
    
    /* Write to StringVal and free the WKT */
    StringVal result(context, wkt_size);
    memcpy(result.ptr, wkt, wkt_size);
    free(wkt);
    
    return result;
}
