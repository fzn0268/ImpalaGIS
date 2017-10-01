/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   geography_measurement.h
 * Author: fanzhongning
 *
 * Created on 2017年9月17日, 下午8:27
 */

#ifndef GEOGRAPHY_MEASUREMENT_H
#define GEOGRAPHY_MEASUREMENT_H

#include <impala_udf/udf.h>

using namespace impala_udf;

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * 
     * @param context UDF context object
     * @param wktText
     * @return double area in meters square
     */
    DoubleVal geography_area_use_spheroid(FunctionContext* context, const StringVal& wktText);
    
    /**
     * 
     * @param context UDF context object
     * @param wktText
     * @param useSpheroid
     * @return double area in meters square
     */
    DoubleVal geography_area(FunctionContext* context, const StringVal& wktText, const BooleanVal& useSpheroid);
    
    /**
     * 
     * @param context
     * @param wktText1
     * @param wktText2
     * @return direction between points (north = 0)
     * azimuth (bearing) and distance
     */
    DoubleVal geography_azimuth(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2);
    
    /**
     * 
     * @param context
     * @param wktText1
     * @param wktText2
     * @return boolean Only works for (multi)points and (multi)polygons currently.
     * Attempts a simple point-in-polygon test on the polygon and point.
     * Current algorithm does not distinguish between points on edge and points within.
     */
    BooleanVal geography_covers(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2);
    
    /**
     * 
     * @param context
     * @param wktText1
     * @param wktText2
     * @param distanceMeters
     * @return double distance in meters
     */
    BooleanVal geography_dwithin_use_spheroid(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2, const DoubleVal& distanceMeters);
    
    /**
     * 
     * @param context
     * @param wktText1
     * @param wktText2
     * @param distanceMeters
     * @param useSpheroid
     * @return double distance in meters
     */
    BooleanVal geography_dwithin(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2, const DoubleVal& distanceMeters, const BooleanVal& useSpheroid);
    
    /**
     * 
     * @param context
     * @param wktText1
     * @param wktText2
     * @return double distance in meters
     */
    DoubleVal geography_distance_use_spheroid(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2);
    
    /**
     * 
     * @param context
     * @param wktText1
     * @param wktText2
     * @param useSpheroid
     * @return double distance in meters
     */
    DoubleVal geography_distance(FunctionContext* context, const StringVal& wktText1, const StringVal& wktText2, const BooleanVal& useSpheroid);
    
    /**
     * 
     * @param context
     * @param wktText
     * @return double length in meters
     */
    DoubleVal geography_length_use_spheroid(FunctionContext* context, const StringVal& wktText);

    /**
     * 
     * @param context
     * @param wktText
     * @param useSpheroid
     * @return double length in meters
     */
    DoubleVal geography_length(FunctionContext* context, const StringVal& wktText, const BooleanVal& useSpheroid);
    
    /**
     * 
     * @param context
     * @param wktText
     * @return double perimeter in meters for area features
     */
    DoubleVal geography_perimeter_use_spheroid(FunctionContext* context, const StringVal& wktText);

    /**
     * 
     * @param context
     * @param wktText
     * @param useSpheroid
     * @return double perimeter in meters for area features
     */
    DoubleVal geography_perimeter(FunctionContext* context, const StringVal& wktText, const BooleanVal& useSpheroid);
    
    /**
     * 
     * @param context
     * @param wktText
     * @param distance
     * @param azimuth
     * @return point of projection given start point, azimuth in radians (bearing) and distance in meters
     */
    StringVal geography_project(FunctionContext* context, const StringVal& wktText, const DoubleVal& distance, const DoubleVal& azimuth);
    
    /**
     * 
     * @param context
     * @param wktText
     * @param distance
     * @param azimuth
     * @return point of projection given start point, azimuth in radians (bearing) and distance in meters
     */
    StringVal geography_segmentize(FunctionContext* context, const StringVal& wktText, const DoubleVal& max_seg_length);
    
#ifdef __cplusplus
}
#endif

#endif /* GEOGRAPHY_MEASUREMENT_H */

