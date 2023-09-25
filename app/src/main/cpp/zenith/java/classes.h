#pragma once

#include <string>

#include <impl_types.h>
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
