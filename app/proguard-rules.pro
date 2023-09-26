# These classes and methods are called through JNI and should not be obfuscated in the release build
-keep class emu.zenith.data.ZenithSettings {
    public java.lang.Object getEnvStateVar(java.lang.String);
}
