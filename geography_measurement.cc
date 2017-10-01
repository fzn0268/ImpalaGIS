/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

extern "C" {
#include <liblwgeom.h>
}

#include "lwgeom_internal.h"
#include "lwgeom_ig.h"
#include "lwgeom_serializer.h"

#include "geography_measurement.h"

#if PROJ_GEODESIC
/* round to 10 nm precision */
#define INVMINDIST 1.0e8
#else
/* round to 100 nm precision */
#define INVMINDIST 1.0e9
#endif

DoubleVal geography_area_use_spheroid(FunctionContext* context, const StringVal& wktText) {
    return geography_area(context, wktText, BooleanVal(true));
}

DoubleVal geography_area(FunctionContext* context, const StringVal& wktText, const BooleanVal& useSpheroid) {
    LWGEOM *lwgeom = NULL;
    GBOX gbox;
    double area;
    bool use_spheroid = useSpheroid.val;
    SPHEROID s;

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    lwgeom = WKTToLWGEOM(context, wktText);

    /* EMPTY things have no area */
    if (lwgeom_is_empty(lwgeom)) {
        lwgeom_free(lwgeom);
        return DoubleVal(0.0);
    }

    if (lwgeom->bbox)
        gbox = *(lwgeom->bbox);
    else
        lwgeom_calculate_gbox_geodetic(lwgeom, &gbox);

#if ! PROJ_GEODESIC
    /* Test for cases that are currently not handled by spheroid code */
    if (use_spheroid) {
        /* We can't circle the poles right now */
        if (FP_GTEQ(gbox.zmax, 1.0) || FP_LTEQ(gbox.zmin, -1.0))
            use_spheroid = LW_FALSE;
        /* We can't cross the equator right now */
        if (gbox.zmax > 0.0 && gbox.zmin < 0.0)
            use_spheroid = LW_FALSE;
    }
#endif /* if ! PROJ_GEODESIC */

    /* User requests spherical calculation, turn our spheroid into a sphere */
    if (!use_spheroid)
        s.a = s.b = s.radius;

    /* Calculate the area */
    if (use_spheroid)
        area = lwgeom_area_spheroid(lwgeom, &s);
    else
        area = lwgeom_area_sphere(lwgeom, &s);

    /* Clean up */
    lwgeom_free(lwgeom);

    /* Something went wrong... */
    if (area < 0.0) {
        context->SetError("lwgeom_area_spher(oid) returned area < 0.0");
        return DoubleVal::null();
    }

    return DoubleVal(area);
}

DoubleVal geography_azimuth(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2) {
    LWGEOM *lwgeom1 = NULL;
    LWGEOM *lwgeom2 = NULL;
    double azimuth;
    SPHEROID s;
    uint32_t type1, type2;

    /* Get our geometry object loaded into memory. */
    lwgeom1 = WKTToLWGEOM(context, wktText1);
    lwgeom2 = WKTToLWGEOM(context, wktText2);

    /* Only return for points. */
    type1 = lwgeom_get_type(lwgeom1);
    type2 = lwgeom_get_type(lwgeom2);
    if (type1 != POINTTYPE || type2 != POINTTYPE) {
        context->SetError("ST_Azimuth(geography, geography) is only valid for point inputs");
        return DoubleVal::null();
    }

    /* EMPTY things cannot be used */
    if (lwgeom_is_empty(lwgeom1) || lwgeom_is_empty(lwgeom2)) {
        lwgeom_free(lwgeom1);
        lwgeom_free(lwgeom2);
        context->SetError("ST_Azimuth(geography, geography) cannot work with empty points");
        return DoubleVal::null();
    }

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    /* Calculate the direction */
    azimuth = lwgeom_azumith_spheroid(lwgeom_as_lwpoint(lwgeom1), lwgeom_as_lwpoint(lwgeom2), &s);

    /* Clean up */
    lwgeom_free(lwgeom1);
    lwgeom_free(lwgeom2);

    /* Return NULL for unknown (same point) azimuth */
    if (isnan(azimuth)) {
        return DoubleVal::null();
    }

    return DoubleVal(azimuth);
}

BooleanVal geography_covers(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2) {
    LWGEOM *lwgeom1 = NULL;
    LWGEOM *lwgeom2 = NULL;
    int result = LW_FALSE;

    /* Construct our working geometries */
    lwgeom1 = WKTToLWGEOM(context, wktText1);
    lwgeom2 = WKTToLWGEOM(context, wktText2);

    error_if_srid_mismatch(lwgeom1->srid, lwgeom2->srid);

    /* EMPTY never intersects with another geometry */
    if (lwgeom_is_empty(lwgeom1) || lwgeom_is_empty(lwgeom2)) {
        lwgeom_free(lwgeom1);
        lwgeom_free(lwgeom2);
        return BooleanVal(false);
    }

    /* Calculate answer */
    result = lwgeom_covers_lwgeom_sphere(lwgeom1, lwgeom2);

    /* Clean up */
    lwgeom_free(lwgeom1);
    lwgeom_free(lwgeom2);

    return BooleanVal(result);
}

BooleanVal geography_dwithin_use_spheroid(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2, const DoubleVal& distanceMeters) {
    return geography_dwithin(context, wktText1, wktText2, distanceMeters, BooleanVal(true));
}

BooleanVal geography_dwithin(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2, const DoubleVal& distanceMeters, const BooleanVal& useSpheroid) {
    LWGEOM *lwgeom1 = NULL;
    LWGEOM *lwgeom2 = NULL;
    double tolerance = 0.0;
    double distance;
    bool use_spheroid = true;
    SPHEROID s;
    bool dwithin = LW_FALSE;

    /* Get our geometry objects loaded into memory. */
    lwgeom1 = WKTToLWGEOM(context, wktText1);
    lwgeom2 = WKTToLWGEOM(context, wktText2);

    /* Read our tolerance value. */
    if (!distanceMeters.is_null)
        tolerance = distanceMeters.val;

    /* Read our calculation type. */
    if (!useSpheroid.is_null)
        use_spheroid = useSpheroid.val;

    error_if_srid_mismatch(lwgeom1->srid, lwgeom2->srid);

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    /* Set to sphere if requested */
    if (!use_spheroid)
        s.a = s.b = s.radius;

    /* Return FALSE on empty arguments. */
    if (lwgeom_is_empty(lwgeom1) || lwgeom_is_empty(lwgeom2)) {
        return BooleanVal(false);
    }

    /* Do the brute force calculation */
    distance = lwgeom_distance_spheroid(lwgeom1, lwgeom2, &s, tolerance);
    /* Something went wrong... */
    if (distance < 0.0)
        context->SetError("lwgeom_distance_spheroid returned negative!");
    dwithin = (distance <= tolerance);
    lwgeom_free(lwgeom1);
    lwgeom_free(lwgeom2);

    return BooleanVal(dwithin);
}

DoubleVal geography_distance_use_spheroid(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2) {
    return geography_distance(context, wktText1, wktText2, BooleanVal(true));
}

DoubleVal geography_distance(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2, const BooleanVal& useSpheroid) {
    LWGEOM *lwgeom1 = NULL;
    LWGEOM *lwgeom2 = NULL;
    double distance;
    double tolerance = 0.0;
    bool use_spheroid = true;
    SPHEROID s;

    /* Get our geometry objects loaded into memory. */
    lwgeom1 = WKTToLWGEOM(context, wktText1);
    lwgeom2 = WKTToLWGEOM(context, wktText2);

    /* Read our calculation type. */
    if (!useSpheroid.is_null)
        use_spheroid = useSpheroid.val;

    error_if_srid_mismatch(lwgeom1->srid, lwgeom2->srid);

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    /* Set to sphere if requested */
    if (!use_spheroid)
        s.a = s.b = s.radius;

    /* Return NULL on empty arguments. */
    if (lwgeom_is_empty(lwgeom1) || lwgeom_is_empty(lwgeom2)) {
        return DoubleVal::null();
    }

    /* Do the brute force calculation */
    distance = lwgeom_distance_spheroid(lwgeom1, lwgeom2, &s, tolerance);
    lwgeom_free(lwgeom1);
    lwgeom_free(lwgeom2);

    /* Knock off any funny business at the nanometer level, ticket #2168 */
    distance = round(distance * INVMINDIST) / INVMINDIST;

    /* Something went wrong, negative return... should already be eloged, return NULL */
    if (distance < 0.0) {
        context->SetError("distance returned negative!");
        return DoubleVal::null();
    }

    return DoubleVal(distance);
}

DoubleVal geography_length_use_spheroid(FunctionContext* context, const StringVal& wktText) {
    return geography_length(context, wktText, BooleanVal(true));
}

DoubleVal geography_length(FunctionContext* context, const StringVal& wktText, const BooleanVal& useSpheroid) {
    LWGEOM *lwgeom = NULL;
    GSERIALIZED *g = NULL;
    double length;
    bool use_spheroid = LW_TRUE;
    SPHEROID s;

    /* Get our geometry object loaded into memory. */
    lwgeom = WKTToLWGEOM(context, wktText);

    /* EMPTY things have no length */
    if (lwgeom_is_empty(lwgeom) || lwgeom->type == POLYGONTYPE || lwgeom->type == MULTIPOLYGONTYPE) {
        lwgeom_free(lwgeom);
        return DoubleVal(0.0);
    }

    /* Read our calculation type */
    if (!useSpheroid.is_null)
        use_spheroid = useSpheroid.val;

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    /* User requests spherical calculation, turn our spheroid into a sphere */
    if (!use_spheroid)
        s.a = s.b = s.radius;

    /* Calculate the length */
    length = lwgeom_length_spheroid(lwgeom, &s);

    /* Something went wrong... */
    if (length < 0.0) {
        context->SetError("lwgeom_length_spheroid returned length < 0.0");
        return DoubleVal::null();
    }

    /* Clean up */
    lwgeom_free(lwgeom);

    return DoubleVal(length);
}

DoubleVal geography_perimeter_use_spheroid(FunctionContext* context, const StringVal& wktText) {
    return geography_perimeter(context, wktText, BooleanVal(true));
}

DoubleVal geography_perimeter(FunctionContext* context, const StringVal& wktText, const BooleanVal& useSpheroid) {
    LWGEOM *lwgeom = NULL;
    double length;
    bool use_spheroid = LW_TRUE;
    SPHEROID s;
    int type;

    /* Get our geometry object loaded into memory. */
    lwgeom = WKTToLWGEOM(context, wktText);

    /* Only return for area features. */
    type = lwgeom_get_type(lwgeom);
    if (!(type == POLYGONTYPE || type == MULTIPOLYGONTYPE || type == COLLECTIONTYPE)) {
        return DoubleVal(0.0);
    }

    /* EMPTY things have no perimeter */
    if (lwgeom_is_empty(lwgeom)) {
        lwgeom_free(lwgeom);
        return DoubleVal(0.0);
    }

    /* Read our calculation type */
    if (!useSpheroid.is_null)
        use_spheroid = useSpheroid.val;

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    /* User requests spherical calculation, turn our spheroid into a sphere */
    if (!use_spheroid)
        s.a = s.b = s.radius;

    /* Calculate the length */
    length = lwgeom_length_spheroid(lwgeom, &s);

    /* Something went wrong... */
    if (length < 0.0) {
        context->SetError("lwgeom_length_spheroid returned length < 0.0");
        return DoubleVal::null();
    }

    /* Clean up, but not all the way to the point arrays */
    lwgeom_free(lwgeom);

    return DoubleVal(length);
}

StringVal geography_project(FunctionContext* context, const StringVal& wktText, const DoubleVal& _distance, const DoubleVal& _azimuth) {
    LWGEOM *lwgeom = NULL;
    LWPOINT *lwp_projected;
    StringVal result = StringVal::null();
    double azimuth = 0.0;
    double distance;
    SPHEROID s;
    uint32_t type;

    /* Return NULL on NULL distance or geography */
    if (wktText.is_null || _distance.is_null)
        return StringVal::null();

    /* Get our geometry object loaded into memory. */
    lwgeom = WKTToLWGEOM(context, wktText);

    /* Only return for points. */
    type = lwgeom_get_type(lwgeom);
    if (type != POINTTYPE) {
        context->SetError("ST_Project(geography) is only valid for point inputs");
        return StringVal::null();
    }

    distance = _distance.val; /* Distance in Meters */

    /* EMPTY things cannot be projected from */
    if (lwgeom_is_empty(lwgeom)) {
        lwgeom_free(lwgeom);
        context->SetError("ST_Project(geography) cannot project from an empty start point");
        return StringVal::null();
    }

    if (!_azimuth.is_null)
        azimuth = _azimuth.val; /* Azimuth in Radians */

    /* Initialize spheroid */
    spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

    /* Handle the zero distance case */
    if (FP_EQUALS(distance, 0.0)) {
        return wktText;
    }

    /* Calculate the length */
    lwp_projected = lwgeom_project_spheroid(lwgeom_as_lwpoint(lwgeom), &s, distance, azimuth);

    /* Something went wrong... */
    if (lwp_projected == NULL) {
        context->SetError("lwgeom_project_spheroid returned null");
        return StringVal::null();
    }

    /* Clean up, but not all the way to the point arrays */
    lwgeom_free(lwgeom);
    result = LWGEOMToEWKT(context, lwpoint_as_lwgeom(lwp_projected));
    lwpoint_free(lwp_projected);

    return result;
}

StringVal geography_segmentize(FunctionContext* context, const StringVal& wktText, const DoubleVal& maxSegLength) {
    LWGEOM *lwgeom1 = NULL;
    LWGEOM *lwgeom2 = NULL;
    StringVal result = StringVal::null();
    double max_seg_length;
    uint32_t type1;

    /* Get our geometry object loaded into memory. */
    lwgeom1 = WKTToLWGEOM(context, wktText);
    type1 = lwgeom_get_type(lwgeom1);

    /* Convert max_seg_length from metric units to radians */
    max_seg_length = maxSegLength.val / WGS84_RADIUS;

    /* We can't densify points or points, reflect them back */
    if (type1 == POINTTYPE || type1 == MULTIPOINTTYPE || lwgeom_is_empty(lwgeom1))
        return wktText;

    /* Calculate the densified geometry */
    lwgeom2 = lwgeom_segmentize_sphere(lwgeom1, max_seg_length);

    /*
     ** Set the geodetic flag so subsequent
     ** functions do the right thing.
     */
    lwgeom_set_geodetic(lwgeom2, true);

    /* Recalculate the boxes after re-setting the geodetic bit */
    lwgeom_drop_bbox(lwgeom2);

    /* We are trusting geography_serialize will add a box if needed */
    result = LWGEOMToEWKT(context, lwgeom2);

    /* Clean up */
    lwgeom_free(lwgeom1);
    lwgeom_free(lwgeom2);

    return result;
}
