# Eyestack 搜索目录，按需修改为不同的配置版本
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(Eyestack_DIR "D:/IICT/build-Eyestack-Desktop_Qt_5_15_2_MSVC2019_64bit-Debug/cmake")
else()
  set(Eyestack_DIR "D:/IICT/build-Eyestack-Desktop_Qt_5_15_2_MSVC2019_64bit-Release/cmake")
endif()

# OpenCV 搜索目录
set(OpenCV_DIR "F:/QT/OpenCv/opencv/build")

# Boost 搜索目录
set(BOOST_ROOT "D:/ICAC/boost_1_76_0")

# 算法库搜索目录
set(GaoCe_DIR "D:/IICT/GaoCe-1.0.2-win64/cmake")
