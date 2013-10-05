cmake_minimum_required(VERSION 2.8)
project(ffxivd)

set(FRAMEWORK_ROOT ../../Framework/)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include_directories(${FRAMEWORK_ROOT}/include /usr/local/include /usr/local/include/c++/v1 /usr/local/include/mysql)
link_directories(../${FRAMEWORK_ROOT}/build_unix/build /usr/local/lib /usr/local/lib/mysql)

add_executable(ffxivd 
	../daemon/AppConfig.cpp
	../daemon/BasePacket.cpp
	../daemon/Character.cpp
	../daemon/CompositePacket.cpp
	../daemon/DatabaseConnectionManager.cpp
	../daemon/GameServer.cpp
	../daemon/GameServerPlayer.cpp
	../daemon/LobbyServer.cpp
	../daemon/LobbyServerPlayer.cpp
	../daemon/Log.cpp
	../daemon/Main.cpp
	../daemon/PacketUtils.cpp
	../daemon/SetInitialPositionPacket.cpp
	../daemon/SetMapPacket.cpp
	../daemon/SetMusicPacket.cpp
	../daemon/SetWeatherPacket.cpp
	../daemon/SocketUtils.cpp
	../common/BLOWFISH.C
)

target_link_libraries(ffxivd Framework Framework_MySql pthread boost_system boost_filesystem mysqlclient)

install(TARGETS ffxivd RUNTIME DESTINATION bin)

