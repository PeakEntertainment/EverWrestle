// EverWrestle - A Peak Entertainment Production (2026)


#include "EverWrestleScoringData.h"

int32 UEverWrestleScoringData::GetPointsForPlacement(const uint8 Placement)
{
	return GetPointsOutOfMap(PlacementPointsMap, Placement);
}

int32 UEverWrestleScoringData::GetPointsForKnockout(const uint8 Knockouts)
{
	return GetPointsOutOfMap(KnockoutPointsMap, Knockouts);
}

int32 UEverWrestleScoringData::GetPointsOutOfMap(const TMap<uint8, int32>& MapToCheck, uint8 Num) const
{
	if (MapToCheck.IsEmpty()) return 0;
	
	Num = FMath::Clamp(Num, 1, static_cast<uint8>(MapToCheck.Num()));
	
	if (const int32 FoundPoints = MapToCheck.FindRef(Num))
	{
		return FoundPoints;
	}
	
	return 0;
}