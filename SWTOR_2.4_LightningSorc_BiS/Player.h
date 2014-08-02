#ifndef PLAYER_H
#define PLAYER_H

#include <string>

using namespace std;

class Player {

	//Setting up variables for raid buffs
	float MARK_OF_POWER_STAT, MARK_OF_POWER_REDUCTION, HUNTERS_BOON, COORDINATION, UNNATURAL_MIGHT;
	
	//Setting up variables for Offensive stats
	float WillpowerRating, TotalWillpower, PowerRating, BonusDamage, BonusDamageFromWillpower, BonusDamageFromSkillsBuffs, BonusDamageFromPower, BonusDamageFromForcePower, ForcePower;
	float AccuracyPercent, WeaponAccuracy, AccuracyFromRating, AccuracyRating, AccuracyFromSkillsBuffs, AccuracyFromCompanion;
	float ForceCriticalChance, BaseCriticalChance, BonusCriticalChance, CritFromRating, CritFromWillpower, CritRating, CritFromSkillsBuffs, CritFromCompanion;
	float SurgePercent, BaseSurge, SurgeFromRating, SurgeRating, SurgeFromSkillsBuffs, SurgeFromCompanion;
	float AlacrityPercent, BaseAlacrity, AlacrityFromRating, AlacrityRating, AlacrityFromSkillsBuffs;

	//Setting up variables for use in the rotation simulation
	float Time;
	unsigned long TotalDamage;

	//Variables for rotation abilities
	float CrushingDarknessDamage, CrushingDarknessCastTime, CrushingDarknessNextCast;
	float AfflictionDamage, AfflictionExpires, Tick1, Tick2, Tick3, Tick4, Tick5, Tick6, TargetAfflictionDebuffed;
	float LightningStrikeDamage, LightningStrikeCastTime;
	float ChainLightningDamage, ChainLightningCastTime, LightningStormNextCast;
	float ForceLightningDamage, ForceLightningCastTime, LightningBarrageNextCast;
	float ThunderingBlastDamage, ThunderingBlastCastTime, ThunderingBlastNextCast;

	//Variables for resistance types
	float EnergyResist, KineticResist, InternalResist;

	//For use in proc-based derivative abilities and effects
	float RandomNumber;

	//Mostly rotation stat tracking to make sure outputs look reasonable
	int LightningBarrage, LightningStorm;
	int RecklessnessCharges, PolarityShift, RecklessnessNextCast, PolarityShiftNextCast, RecklessnessCasts, PolarityShiftCasts, PolarityShiftExpires;
	int CrushingDarknessCasts, AfflictionCasts,	ForceLightningCasts, ChainLightningCasts, ThunderingBlastCasts,	LightningStrikeCasts;

	//Variables for relic buffs
	int	PowerSurge,	PowerSurgeNextCast,	PowerSurgeExpires, PowerSurgeCasts;
	int PowerBoost,	PowerBoostNextCast,	PowerBoostExpires, PowerBoostCasts;

	//Player 4-piece set bonus buff
	int ForceMasterAlacrity, ForceMasterAlacrityNextCast, ForceMasterAlacrityExpires, ForceMasterAlacrityCasts;

public:

	//Variable to toggle raid buffs and companion buffs; STDHP is the Standard Health value used in determining spell damage
	short IN_GROUP, COMP_HEALTH_BUFF, COMP_CRIT_BUFF, COMP_SURGE_BUFF, COMP_ACCURACY_BUFF;
	float STDHP;
	
	Player ();

	//Declaring member function prototypes to initialize player offensive stats
	//modify them, and display them at the end of the program.
	float modOffStats (string, float, string, float, string, float, string, float, string, float, string, float, short);
	void showOffStats ();

	//These member functions will go through each piece of gear of their respective
	//types to determine BiS gear for a given rotation.
	void OffHiltArmorings (float []);
	void OffMods (float []);
	void OffEnhancements (float []);
	void OffEarpieces (float []);
	void OffImplants (float []);
	void OffColorCrystals (float []);
	void OffAugments (float []);

	//Function prototypes for Lightning Sorc abilities
	float CastCrushingDarkness();
	float CastLightningStrike();
	float CastChainLightning();
	float CastForceLightning();
	void CastAffliction();
	void AfflictionTick();
	float CastThunderingBlast();

	//Function prototypes for Sorc self-buffs
	void CastRecklessness();
	void CastPolarityShift();

	//Function prototypes for relics
	void CastPowerSurge();
	void CastPowerBoost();

	//Function prototype for Sorc 4-piece bonus
	void CastForceMasterAlacrity();

	//Rotation and stats
	float Rotation();
	void DisplayRotationStats();

	//This is never called, but I made this to make setting up the gear
	//comparison structures much less time consuming by copy and paste.
	//In retrospect, I should have just written the template in a commented
	//out section of this code instead of writing a pointless member function.
	void Template(float []);

};


#endif