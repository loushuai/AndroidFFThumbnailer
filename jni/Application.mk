APP_ABI := armeabi-v7a

NDK_TOOLCHAIN_VERSION := clang

#APP_STL := stlport_static
APP_STL := c++_static
APP_CPPFLAGS += -frtti
#APP_STL := gnustl_static