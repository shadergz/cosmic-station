# These classes and methods are called through JNI and should not be obfuscated in the release build
-keep class emu.zenith.data.ZenithSettings {
    public java.lang.Object getDataStoreValue(java.lang.String);
}
-keep class emu.zenith.data.BiosModel
-keep class emu.zenith.data.DriverMeta*

-keep class emu.zenith.fragments.GlobalSettingsFragment
-keep class emu.zenith.fragments.GraphicsSettingsFragment
