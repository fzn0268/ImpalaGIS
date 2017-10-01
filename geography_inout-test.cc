/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>

#include <impala_udf/udf-test-harness.h>
#include "geography_inout.h"

using namespace impala;
using namespace impala_udf;
using namespace std;

int main(int argc, char** argv) {
    bool passed = true;
    
    passed &= UdfTestHarness::ValidateUdf<StringVal, StringVal>(geography_from_text, StringVal("SRID=4267;POINT(-77.0092 38.889588)"), StringVal("0101000020AB100000E3C798BB964053C000750305DE714340\0"));

    const std::vector<AnyVal*>& constant_args = std::vector<AnyVal*>();
    
    FunctionContext::TypeDesc return_type; // TODO
    std::vector<FunctionContext::TypeDesc> arg_types; // TODO
    boost::scoped_ptr<FunctionContext> context(UdfTestHarness::CreateTestContext(return_type, arg_types));
    UdfTestHarness::SetConstantArgs(context.get(), constant_args);
    
    StringVal ret = geography_from_text(context.get(), StringVal("SRID=4267;POINT(-77.0092 38.889588)"));
    
    cout << "Tests " << (passed ? "Passed." : "Failed.") << "\nResult: " << (char*) ret.ptr << endl;
    return !passed;
}