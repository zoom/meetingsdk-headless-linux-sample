cmake_minimum_required(VERSION 3.20.2)
project(meeting_sdk_linux_sample VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(ZOOM_SDK lib/zoomsdk)

#find_package(ada REQUIRED)
#find_package(CLI11 REQUIRED)
#find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

find_package(google_cloud_cpp_common CONFIG REQUIRED)
find_package(google_cloud_cpp_googleapis CONFIG REQUIRED)
find_package(google_cloud_cpp_grpc_utils CONFIG REQUIRED)
find_package(google_cloud_cpp_language CONFIG REQUIRED)
find_package(google_cloud_cpp_speech CONFIG REQUIRED)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

include_directories(${ZOOM_SDK}/h)
link_directories(${ZOOM_SDK} ${ZOOM_SDK})
link_directories(${ZOOM_SDK} ${ZOOM_SDK}/qt_libs/**)

add_executable(zoomsdk src/main.cpp)

target_include_directories(zoomsdk PRIVATE ${JWT_CPP_INCLUDE_DIRS})
target_link_libraries(zoomsdk PRIVATE google-cloud-cpp::common google-cloud-cpp::grpc_utils gRPC::grpc gRPC::grpc++ google-cloud-cpp::api_log_protos google-cloud-cpp::api_auth_protos google-cloud-cpp::speech google-cloud-cpp::speech_protos google-cloud-cpp::cloud_speech_protos google-cloud-cpp::language google-cloud-cpp::language_protos)






