#pragma once

#include <string>

namespace zenith::java {

    using JNIEnumerator = uint;

    class JNIString {
    public:
        JNIString() : managedStr() {}

        std::string managedStr;
    };

}
