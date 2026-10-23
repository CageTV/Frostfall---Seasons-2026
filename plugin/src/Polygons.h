#pragma once

// Region outlines from Frostfall - Seasons 1.5 by BrinaSair (DLC and new-land regions, the Throat of the World) and Frostfall 3.4.1 by Chesko (Falmer Valley, Darkend, Wyrmstooth).
// x / y in game units of the worldspace the region belongs to.

namespace Polygons
{
	struct Polygon
	{
		const float* x;
		const float* y;
		std::size_t  count;
	};

	inline constexpr float kThroatX[] = { 38910.00f, 26625.00f, 32800.00f, 36865.00f, 49150.00f, 57345.00f, 69630.00f, 69630.00f, 73730.00f, 69630.00f, 65535.00f, 57345.00f, 53250.00f, 49150.00f };
	inline constexpr float kThroatY[] = { -20480.00f, -53250.00f, -69630.00f, -69630.00f, -79870.00f, -77825.00f, -69630.00f, -61440.00f, -57345.00f, -24575.00f, -20480.00f, -20480.00f, -24575.00f, -20480.00f };
	inline constexpr Polygon kThroat{ kThroatX, kThroatY, 14 };

	inline constexpr float kFalmerBossX[] = { -7340.00f, -7065.00f, -12013.00f, -14487.00f, -11023.00f };
	inline constexpr float kFalmerBossY[] = { -7244.00f, -9662.00f, -14170.00f, -10432.00f, -7244.00f };
	inline constexpr Polygon kFalmerBoss{ kFalmerBossX, kFalmerBossY, 5 };

	inline constexpr float kFalmerIceX[] = { -22417.00f, 5287.00f, 17525.00f, 38431.00f, 47312.00f, 16369.00f, -4083.00f, -24967.00f };
	inline constexpr float kFalmerIceY[] = { 15633.00f, 16908.00f, -2680.00f, 10491.00f, -10372.00f, -43088.00f, -16276.00f, -6122.00f };
	inline constexpr Polygon kFalmerIce{ kFalmerIceX, kFalmerIceY, 8 };

	inline constexpr float kFalmerCanyonX[] = { -47273.00f, -4083.00f, 16369.00f, -31458.00f, -55597.00f };
	inline constexpr float kFalmerCanyonY[] = { 15175.00f, -16276.00f, -43088.00f, -43787.00f, -18209.00f };
	inline constexpr Polygon kFalmerCanyon{ kFalmerCanyonX, kFalmerCanyonY, 5 };

	inline constexpr float kFalmerPlaygroundX[] = { -47273.00f, -42686.00f, 24409.00f, 5287.00f, -22417.00f, -24967.00f };
	inline constexpr float kFalmerPlaygroundY[] = { 15175.00f, 40024.00f, 50095.00f, 16908.00f, 15633.00f, -6122.00f };
	inline constexpr Polygon kFalmerPlayground{ kFalmerPlaygroundX, kFalmerPlaygroundY, 6 };

	inline constexpr float kFalmerGroveX[] = { 24409.00f, 46535.00f, 58431.00f, 17525.00f, 5287.00f };
	inline constexpr float kFalmerGroveY[] = { 50095.00f, 17714.00f, 10491.00f, -2680.00f, 16908.00f };
	inline constexpr Polygon kFalmerGrove{ kFalmerGroveX, kFalmerGroveY, 5 };

	inline constexpr float kSolstheimArcticX[] = { 19400.00f, 23549.00f, 23854.00f, 23038.00f, 20764.00f, 25385.00f, 28895.00f, 28665.00f, 33574.00f, 37186.00f, 72619.00f, 72922.00f, 78100.00f, 79563.00f, 107700.00f, 107700.00f, 2000.00f, 2000.00f };
	inline constexpr float kSolstheimArcticY[] = { 77192.00f, 75957.00f, 71695.00f, 64635.00f, 58523.00f, 57941.00f, 50985.00f, 46571.00f, 47793.00f, 49747.00f, 54769.00f, 60439.00f, 60496.00f, 63665.00f, 64000.00f, 106000.00f, 106000.00f, 77200.00f };
	inline constexpr Polygon kSolstheimArctic{ kSolstheimArcticX, kSolstheimArcticY, 18 };

	inline constexpr float kDarkendFrozenX[] = { 931.00f, 19951.00f, 28261.00f, -4750.00f };
	inline constexpr float kDarkendFrozenY[] = { 14542.00f, 6015.00f, 49880.00f, 49880.00f };
	inline constexpr Polygon kDarkendFrozen{ kDarkendFrozenX, kDarkendFrozenY, 4 };

	inline constexpr float kWyrmstoothCenterX[] = { 19045.20f, 13357.80f, 19684.60f, 41285.80f, 25999.50f };
	inline constexpr float kWyrmstoothCenterY[] = { 727.36f, -10888.50f, -18279.10f, 3264.52f, 8109.01f };
	inline constexpr Polygon kWyrmstoothCenter{ kWyrmstoothCenterX, kWyrmstoothCenterY, 5 };

	inline constexpr float kWyrmstoothWestX[] = { -184.00f, -6402.00f, -16690.00f, -14528.00f, -10068.00f, -11671.00f, -70148.00f, -63280.00f };
	inline constexpr float kWyrmstoothWestY[] = { 17652.00f, -3939.00f, -8281.00f, -17718.00f, -29543.00f, -18706.00f, -29523.00f, 52329.00f };
	inline constexpr Polygon kWyrmstoothWest{ kWyrmstoothWestX, kWyrmstoothWestY, 8 };

}
