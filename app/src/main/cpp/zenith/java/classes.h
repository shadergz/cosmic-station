#pragma once

#include <string>

namespace zenith::java {

    using JNIEnumerator = uint;

    class JNIString {
    public:
        JNIString()
            : m_managedStr()
            {}

        std::string m_managedStr;
    };

}
