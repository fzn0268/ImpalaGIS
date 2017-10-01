/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include <sstream>
#include <string>

extern "C" {
#include <liblwgeom.h>
}

#include "lwgeom_ig.h"

void ig_parser_errhint(FunctionContext* context, LWGEOM_PARSER_RESULT *lwg_parser_result)
{
    char *hintbuffer;
    std::ostringstream error_string_stream;

    /* Only display the parser position if the location is > 0; this provides a nicer output when the first token
       within the input stream cannot be matched */
    if (lwg_parser_result->errlocation > 0) {
        /* Return a copy of the input string start truncated
         * at the error location */
        hintbuffer = lwmessage_truncate(
                (char *) lwg_parser_result->wkinput, 0,
                lwg_parser_result->errlocation - 1, 40, 0);
        
        error_string_stream << lwg_parser_result->message << "\n\"" << hintbuffer << "\" <-- parse error at position " << lwg_parser_result->errlocation << " within geometry" << std::endl;

        context->SetError(error_string_stream.str().c_str());
    } else {
        error_string_stream << lwg_parser_result->message << "\n\"" << "You must specify a valid OGC WKT geometry type such as POINT, LINESTRING or POLYGON" << std::endl;
    }
}

void ig_unparser_errhint(FunctionContext* context, LWGEOM_UNPARSER_RESULT *lwg_unparser_result)
{
    context->SetError(lwg_unparser_result->message);
}
