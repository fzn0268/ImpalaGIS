/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

extern "C" {
#include <liblwgeom.h>
}

#include "lwgeom_serializer.h"
#include "lwgeom_functions_basic.h"

StringVal LWGEOM_asEWKT(FunctionContext* context, const StringVal& hexwkb)
{
    LWGEOM *lwgeom = HexWKBToLWGEOM(hexwkb);
    
    StringVal result = LWGEOMToEWKT(context, lwgeom);
    lwgeom_free(lwgeom);
    return result;
}

StringVal LWGEOM_summary(FunctionContext* context, const StringVal& wktText) {
    char *result;
    StringVal mystringval = StringVal::null();
    LWGEOM *lwgeom;

    lwgeom = WKTToLWGEOM(context, wktText);
    result = lwgeom_summary(lwgeom, 0);
    lwgeom_free(lwgeom);

    /* create a text obj to return */
    mystringval = StringVal::CopyFrom(context, (uint8_t*) result, strlen(result));
    free(result);

    return mystringval;
}
