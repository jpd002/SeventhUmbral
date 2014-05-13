#pragma once

#include "BasePacket.h"

class CSetWeatherPacket : public CBasePacket
{
public:
	enum WEATHER_ID
	{
		WEATHER_CLEAR	= 0x011F41,
		WEATHER_FINE,
		WEATHER_CLOUDY,
		WEATHER_FOGGY,
		WEATHER_WINDY,				//NOT SUPPORTED in v1.23
		WEATHER_BLUSTERY,
		WEATHER_RAINY,
		WEATHER_SHOWERY,			//NOT SUPPORTED in v1.23
		WEATHER_THUNDERY,			//NOT SUPPORTED in v1.23
		WEATHER_STORMY,
		WEATHER_DUSTY,				//NOT SUPPORTED in v1.23
		WEATHER_SANDY,
		WEATHER_HOT,				//NOT SUPPORTED in v1.23
		WEATHER_BLISTERING,			//NOT SUPPORTED in v1.23
		WEATHER_SNOWY,				//NOT SUPPORTED in v1.23
		WEATHER_WINTRY,				//NOT SUPPORTED in v1.23
		WEATHER_GLOOMY,
		WEATHER_DALAMUD	= 0x011F5E,
	};

							CSetWeatherPacket();
	virtual					~CSetWeatherPacket();

	virtual PacketPtr		Clone() const override;

	virtual PacketData		ToPacketData() const override;

	void					SetWeatherId(uint32);

private:
	uint32					m_weatherId;
};
