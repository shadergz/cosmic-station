#pragma once

#include <jni.h>
#include <common/types.h>
namespace cosmic::java {
    struct CosmicEnv {
        CosmicEnv(CosmicEnv&) = delete;
        CosmicEnv(CosmicEnv&&) = delete;
        CosmicEnv() {
            if (jvm) {
                jvm->GetEnv(BitCast<void**>(&gnv), jv);
            }
            if (!gnv && jvm) {
                jvm->AttachCurrentThread(&gnv, nullptr);
                attached = true;
            }
        }
        void feedVm(JNIEnv* e) {
            if ((jv = e->GetVersion()) != JNI_VERSION_1_6)
                ;
            if (!jvm)
                e->GetJavaVM(&jvm);
            gnv = e;
        }
        void reload() {
            jvm->GetEnv(BitCast<void**>(&gnv), jv);
        }
        void reload(JNIEnv* e) {
            if (!gnv)
                feedVm(e);
            else
                gnv = e;
        }
        ~CosmicEnv() {
            if (attached) {
                jvm->DetachCurrentThread();
            }
        }
        auto operator*() {
            return gnv;
        }
        auto operator->() {
            return gnv;
        }

        static inline JavaVM* jvm{};
        static inline i32 jv;
        // Means that it was created by a native thread
        bool attached{};
        JNIEnv* gnv{};
    };
}