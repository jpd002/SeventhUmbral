cmake_minimum_required(VERSION 2.8)
project(ffxivd)

set(FRAMEWORK_ROOT ../../Framework/)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include_directories(${FRAMEWORK_ROOT}/include /usr/local/include)
link_directories(../${FRAMEWORK_ROOT}/build_unix/build /usr/local/lib)

add_executable(ffxivd 
	../daemon/BasePacket.cpp
	../daemon/Character.cpp
	../daemon/CompositePacket.cpp
	../daemon/GameServer.cpp
	../daemon/GameServerPlayer.cpp
	../daemon/LobbyServer.cpp
	../daemon/Log.cpp
	../daemon/Main.cpp
	../daemon/PacketUtils.cpp
	../daemon/SetInitialPositionPacket.cpp
	../daemon/SetMapPacket.cpp
	../daemon/SetMusicPacket.cpp
	../daemon/SetWeatherPacket.cpp
	../common/BLOWFISH.C
)

target_link_libraries(ffxivd Framework pthread boost_system boost_filesystem)

