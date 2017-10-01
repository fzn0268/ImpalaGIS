/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

extern "C" {
#include <liblwgeom.h>
}

#include "lwgeom_ig.h"
#include "lwgeom_serializer.h"
#include "geography_inout.h"

StringVal geography_from_text(FunctionContext* context, const StringVal& wktText) {
    LWGEOM* lwgeom;
    char* hexwkb;
    size_t hexwkb_length;
    
    lwgeom = WKTToLWGEOM(context, wktText);

    /* TODO Error on any SRID != default */
    //srid_is_latlong(fcinfo, lwg_parser_result.geom->srid);

    /* Clean up string */
    hexwkb = lwgeom_to_hexwkb(lwgeom, WKB_EXTENDED, &hexwkb_length);

    /* Clean up temporary object */
    lwgeom_free(lwgeom);
        
    StringVal result(context, hexwkb_length - 1);
    memcpy(result.ptr, hexwkb, hexwkb_length);
    free(hexwkb);
    return result;
}
