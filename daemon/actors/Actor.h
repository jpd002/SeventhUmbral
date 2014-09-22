#pragma once

#include <boost/signals2.hpp>
#include "Types.h"
#include "../PacketUtils.h"
#include "../packets/BasePacket.h"

class CInstance;

class CActor
{
public:
	typedef boost::signals2::signal<void (const PacketData&)> RawPacketReadySignal;
	typedef boost::signals2::signal<void (CActor*, const PacketPtr&)> LocalPacketReadySignal;
	typedef boost::signals2::signal<void (CActor*, const PacketPtr&)> GlobalPacketReadySignal;

								CActor();
	virtual						~CActor();

	CInstance*					GetInstance() const;
	void						SetInstance(CInstance*);

	uint32						GetId() const;
	void						SetId(uint32);

	void						SetZoneId(uint32);

	void						SetHp(uint32);

	virtual void				Update(float);

	virtual void				TakeDamage(CActor*, uint32);

	RawPacketReadySignal		RawPacketReady;
	LocalPacketReadySignal		LocalPacketReady;
	GlobalPacketReadySignal		GlobalPacketReady;

protected:
	CInstance*					m_instance = nullptr;
	uint32						m_id = 0;
	uint32						m_hp = 0;
	uint32						m_zoneId = 0;

	void						SendHpUpdate();
};
