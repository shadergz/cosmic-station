#pragma once

#include <impl_types.h>
#include <string>

namespace zenith::java {

    using JNIEnumerator = u32;

    class JNIString {
    public:
        JNIString()
            : m_managedStr()
            {}

        std::string m_managedStr;
    };

}
