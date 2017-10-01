/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   geography_measurement-test.cc
 * Author: fanzhongning
 *
 * Created on 2017年10月1日, 下午4:56
 */

#include <iostream>

#include <impala_udf/udf-test-harness.h>
#include "geography_measurement.h"

using namespace impala;
using namespace impala_udf;
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    bool passed = true;

    const std::vector<AnyVal*>& constant_args = std::vector<AnyVal*>();
    
    FunctionContext::TypeDesc return_type; // TODO
    std::vector<FunctionContext::TypeDesc> arg_types; // TODO
    boost::scoped_ptr<FunctionContext> context(UdfTestHarness::CreateTestContext(return_type, arg_types));
    UdfTestHarness::SetConstantArgs(context.get(), constant_args);
    
    DoubleVal area_use_spheroid = geography_area_use_spheroid(context.get(), StringVal("SRID=4267;POLYGON((-71.1776585052917 42.3902909739571,-71.1776820268866 42.3903701743239,-71.1776063012595 42.3903825660754, -71.1775826583081 42.3903033653531, -71.1776585052917 42.3902909739571))"));
    DoubleVal area = geography_area(context.get(), StringVal("SRID=4267;POLYGON((-71.1776585052917 42.3902909739571,-71.1776820268866 42.3903701743239,-71.1776063012595 42.3903825660754, -71.1775826583081 42.3903033653531, -71.1776585052917 42.3902909739571))"), BooleanVal(false));

    cout << "Tests " << (passed ? "Passed." : "Failed.") << "\nResult use spheroid: " << area_use_spheroid.val << "\nResult: " << area.val << endl;
    
    return 0;
}

