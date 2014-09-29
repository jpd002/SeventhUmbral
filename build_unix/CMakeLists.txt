cmake_minimum_required(VERSION 2.8)
project(ffxivd)

set(FRAMEWORK_ROOT ../../Framework/)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

include_directories(${FRAMEWORK_ROOT}/include /usr/local/include /usr/local/include/c++/v1 /usr/local/include/mysql)
link_directories(../${FRAMEWORK_ROOT}/build_unix/build /usr/local/lib /usr/local/lib/mysql)

add_executable(ffxivd 
	../daemon/actors/Actor.cpp
	../daemon/actors/EnemyActor.cpp
	../daemon/actors/PlayerActor.cpp
	../daemon/AppConfig.cpp
	../daemon/AppearanceDatabase.cpp
	../daemon/Character.cpp
	../daemon/Clock.cpp
	../daemon/DatabaseConnectionManager.cpp
	../daemon/GameServer.cpp
	../daemon/GameServerPlayer.cpp
	../daemon/GlobalData.cpp
	../daemon/Instance.cpp
	../daemon/LobbyServer.cpp
	../daemon/LobbyServerPlayer.cpp
	../daemon/Log.cpp
	../daemon/Main.cpp
	../daemon/packets/BasePacket.cpp
	../daemon/packets/BattleActionPacket.cpp
	../daemon/packets/ChangeEquipmentSlotPacket.cpp
	../daemon/packets/CommandRequestReplyPacket.cpp
	../daemon/packets/CompositePacket.cpp
	../daemon/packets/DisplayMessagePacket.cpp
	../daemon/packets/FinishScriptPacket.cpp
	../daemon/packets/KeepAlivePacket.cpp
	../daemon/packets/SetActorAppearancePacket.cpp
	../daemon/packets/SetActorPropertyPacket.cpp
	../daemon/packets/SetActorStatePacket.cpp
	../daemon/packets/SetInitialPositionPacket.cpp
	../daemon/packets/SetInventoryPacket.cpp
	../daemon/packets/SetMapPacket.cpp
	../daemon/packets/SetMusicPacket.cpp
	../daemon/packets/SetTempInventoryPacket.cpp
	../daemon/packets/SetWeatherPacket.cpp
	../daemon/packets/UnknownInventoryPacket.cpp
	../daemon/PacketUtils.cpp
	../daemon/SocketUtils.cpp
	../daemon/ZoneDatabase.cpp
	../common/BLOWFISH.C
)

target_link_libraries(ffxivd cxxrt Framework Framework_MySql pthread boost_system boost_filesystem mysqlclient z)

install(TARGETS ffxivd RUNTIME DESTINATION bin)

