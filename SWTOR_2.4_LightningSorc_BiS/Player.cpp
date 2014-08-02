#include <iostream>
#include "Player.h"

using namespace std;

Player::Player () {

	Time = 0;
	TotalDamage = 0;

	STDHP = 11180;

	CrushingDarknessCasts = 0;
	AfflictionCasts = 0;
	ForceLightningCasts = 0;
	ChainLightningCasts = 0;
	ThunderingBlastCasts = 0;
	LightningStrikeCasts = 0;

	AfflictionExpires = 0;
	TargetAfflictionDebuffed = 0;
	Tick1 = 0;
	Tick2 = 0;
	Tick3 = 0;
	Tick4 = 0;
	Tick5 = 0;
	Tick6 = 0;

	//These values reflect simulated target resistences.
	InternalResist = 0;
	EnergyResist = .3;
	KineticResist = .35;

	//Group buffs
	if (IN_GROUP == 1) {
		COORDINATION = .05;
		UNNATURAL_MIGHT = 1.05;
		MARK_OF_POWER_STAT = .05;
	}

	else {
		COORDINATION = 0.0;
		UNNATURAL_MIGHT = 1.0;
		MARK_OF_POWER_STAT = 0.0;
	}

	//Companion buffs
	if (COMP_CRIT_BUFF == 1)
		CritFromCompanion = .01;

	else
		CritFromCompanion = 0.0;

	if (COMP_SURGE_BUFF == 1)
		SurgeFromCompanion = .01;

	else
		SurgeFromCompanion = 0.0;

	if (COMP_ACCURACY_BUFF == 1)
		AccuracyFromCompanion = .01;

	else
		AccuracyFromCompanion = 0.0;
		
	//Accuracy
	AccuracyRating = 0.0;
	AccuracyFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , AccuracyRating / 55.0 / 1.2 ));
	AccuracyFromSkillsBuffs = 1.03 + AccuracyFromCompanion;
	AccuracyPercent = AccuracyFromRating + AccuracyFromSkillsBuffs;

	//BonusDamage
	WillpowerRating = 400.0;
	TotalWillpower = WillpowerRating * (1 + MARK_OF_POWER_STAT + .06);
	BonusDamageFromWillpower = TotalWillpower * .2;
	PowerRating = 0.0;
	BonusDamageFromPower = PowerRating * .23;
	ForcePower = 2012.0;
	BonusDamageFromForcePower = ForcePower * .23;
	BonusDamage = (BonusDamageFromWillpower + BonusDamageFromPower + BonusDamageFromForcePower) * UNNATURAL_MIGHT * 1.06;

	//Crit
	CritRating = 0.0;
	CritFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , CritRating / 55.0 / .9 ));
	CritFromWillpower = .2 * ( 1.0 - pow( 1.0 - (.01 / .2) , TotalWillpower / 55.0 / 5.5 ));
	CritFromSkillsBuffs = CritFromCompanion + .01 + COORDINATION;
	BaseCriticalChance = .05;
	ForceCriticalChance = CritFromRating + CritFromWillpower + CritFromSkillsBuffs + BaseCriticalChance;
	
	//Surge
	SurgeRating = 0.0;
	SurgeFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , SurgeRating / 55.0 / .22 ));
	SurgeFromSkillsBuffs = SurgeFromCompanion;
	BaseSurge = .5;
	SurgePercent = SurgeFromRating + SurgeFromSkillsBuffs + BaseSurge;

	//Alacrity
	AlacrityRating = 0.0;
	AlacrityFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , AlacrityRating / 55.0 / 1.25 ));
	AlacrityFromSkillsBuffs = 0.02;
	BaseAlacrity = 0.0;
	AlacrityPercent = BaseAlacrity + AlacrityFromSkillsBuffs + AlacrityFromRating;
}

float Player::CastForceLightning() {

	/*
	When Force Lightning is cast, Lightning Barrage is consumed. The cast time is determined
	by the talented cast time of 1.5sec, modified by alacrity. Time is then incremented by
	Force Lightning's cast time.  If there are any Reckless charges available, one will be
	consumed to increase the crit chance of the ability. Force Lightning then has a 30% chance
	to auto-proc Power Surge, the power-increasing relic. A Force Lightning cast is then added
	to the tally to verify that this ability is being used a reasonable amount of times. The
	return statement then takes the Force Lightning damage and modifies it according to the
	target's energy resistance value. Against an OPs boss, this is 30%. This damage value is
	later added to the Total Damage value which ultimately determines which piece of gear
	to select.
	*/
	LightningBarrage = 0;
	ForceLightningCastTime = 1.5 * (1 - AlacrityPercent);
	Time += ForceLightningCastTime;

	if (RecklessnessCharges != 0) {
		ForceLightningDamage = (1 + (ForceCriticalChance + .06 + .6) * SurgePercent) * (.0805 * STDHP + 3.35 * BonusDamage);
		RecklessnessCharges--;
	}

	else
		ForceLightningDamage = (1 + (ForceCriticalChance + .06) * SurgePercent) * (.0805 * STDHP + 3.35 * BonusDamage);

	if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
		CastPowerSurge();

	ForceLightningCasts++;

	return ForceLightningDamage * (1 - EnergyResist);

}

void Player::CastAffliction() {

	TargetAfflictionDebuffed = 1;

	Tick1 = Time + 3;
	Tick2 = Time + 6;
	Tick3 = Time + 9;
	Tick4 = Time + 12;
	Tick5 = Time + 15;
	Tick6 = Time + 18;

	AfflictionExpires = Time + 18;
	AfflictionCasts++;
	Time += 1.5;

}

void Player::AfflictionTick() {

	if (Time > Tick1) {

		AfflictionDamage = (1 + ForceCriticalChance * SurgePercent) * (.055 * STDHP + 2.289 * BonusDamage) / 6;
		TotalDamage += AfflictionDamage * (1 - InternalResist);
		RandomNumber = rand() % 10000;

		Tick1 += 100;

		if (LightningBarrageNextCast <= Time && RandomNumber < ForceCriticalChance * 10000) {
			LightningBarrage = 1;
			LightningBarrageNextCast = Time + 10;
		}

		if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
			CastPowerSurge();

		if (ForceMasterAlacrityNextCast <= Time && rand() % 100 < 30)
			CastForceMasterAlacrity();

	}

	if (Time > Tick2) {

		AfflictionDamage = (1 + ForceCriticalChance * SurgePercent) * (.055 * STDHP + 2.289 * BonusDamage) / 6;
		TotalDamage += AfflictionDamage * (1 - InternalResist);
		RandomNumber = rand() % 10000;

		Tick2 += 100;

		if (LightningBarrageNextCast <= Time && RandomNumber < ForceCriticalChance * 10000) {
			LightningBarrage = 1;
			LightningBarrageNextCast = Time + 10;
		}

		if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
			CastPowerSurge();

		if (ForceMasterAlacrityNextCast <= Time && rand() % 100 < 30)
			CastForceMasterAlacrity();

	}

	if (Time > Tick3) {

		AfflictionDamage = (1 + ForceCriticalChance * SurgePercent) * (.055 * STDHP + 2.289 * BonusDamage) / 6;
		TotalDamage += AfflictionDamage * (1 - InternalResist);
		RandomNumber = rand() % 10000;

		Tick3 += 100;

		if (LightningBarrageNextCast <= Time && RandomNumber < ForceCriticalChance * 10000) {
			LightningBarrage = 1;
			LightningBarrageNextCast = Time + 10;
		}

		if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
			CastPowerSurge();

		if (ForceMasterAlacrityNextCast <= Time && rand() % 100 < 30)
			CastForceMasterAlacrity();

	}

	if (Time > Tick4) {

		AfflictionDamage = (1 + ForceCriticalChance * SurgePercent) * (.055 * STDHP + 2.289 * BonusDamage) / 6;
		TotalDamage += AfflictionDamage * (1 - InternalResist);
		RandomNumber = rand() % 10000;

		Tick4 += 100;

		if (LightningBarrageNextCast <= Time && RandomNumber < ForceCriticalChance * 10000) {
			LightningBarrage = 1;
			LightningBarrageNextCast = Time + 10;
		}

		if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
			CastPowerSurge();

		if (ForceMasterAlacrityNextCast <= Time && rand() % 100 < 30)
			CastForceMasterAlacrity();

	}

	if (Time > Tick5) {

		AfflictionDamage = (1 + ForceCriticalChance * SurgePercent) * (.055 * STDHP + 2.289 * BonusDamage) / 6;
		TotalDamage += AfflictionDamage * (1 - InternalResist);
		RandomNumber = rand() % 10000;

		Tick5 += 100;

		if (LightningBarrageNextCast <= Time && RandomNumber < ForceCriticalChance * 10000) {
			LightningBarrage = 1;
			LightningBarrageNextCast = Time + 10;
		}

		if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
			CastPowerSurge();

		if (ForceMasterAlacrityNextCast <= Time && rand() % 100 < 30)
			CastForceMasterAlacrity();

	}

	if (Time > Tick6) {

		AfflictionDamage = (1 + ForceCriticalChance * SurgePercent) * (.055 * STDHP + 2.289 * BonusDamage) / 6;
		TotalDamage += AfflictionDamage * (1 - InternalResist);
		RandomNumber = rand() % 10000;

		Tick6 += 100;

		if (LightningBarrageNextCast <= Time && RandomNumber < ForceCriticalChance * 10000) {
			LightningBarrage = 1;
			LightningBarrageNextCast = Time + 10;
		}

		if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
			CastPowerSurge();

		if (ForceMasterAlacrityNextCast <= Time && rand() % 100 < 30)
			CastForceMasterAlacrity();

		TargetAfflictionDebuffed = 0;
	}
}

float Player::CastThunderingBlast() {

	ThunderingBlastCastTime = 2 * (1 - AlacrityPercent);
	Time += ThunderingBlastCastTime;

	ThunderingBlastDamage = (1 + .3 * .3) * (1 + 1 * (SurgePercent + .5)) * (.0445 * STDHP + 1.862 * BonusDamage);

	ThunderingBlastCasts++;

	ThunderingBlastNextCast = Time + 9;

	if (LightningStormNextCast <= Time && rand() % 100 < 60) {
		LightningStorm = 1;
		LightningStormNextCast = Time + 10;
	}

	if (PolarityShift == 0 && rand() % 100 < 30)
		PolarityShiftNextCast--;

	if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
		CastPowerSurge();

	return ThunderingBlastDamage * InternalResist;

}

float Player::CastChainLightning() {

	ChainLightningCastTime = 1.5 * (1 - AlacrityPercent);

	ChainLightningCasts++;
	LightningStorm = 0;
	Time += ChainLightningCastTime;

	if (RecklessnessCharges != 0) {
		ChainLightningDamage = (1 + .03 * .03) * (1 + (ForceCriticalChance + .6) * (SurgePercent + .5)) * (.055 * STDHP + 2.3 * BonusDamage);
		RecklessnessCharges--;
	}
	
	else
		ChainLightningDamage = (1 + .03 * .03) * (1 + ForceCriticalChance * (SurgePercent + .5)) * (.055 * STDHP + 2.3 * BonusDamage);

	if (PolarityShift == 0 && rand() % 100 < 30)
		PolarityShiftNextCast--;

	else if (PolarityShift == 1 && rand() % 100 < 60)
		PolarityShiftNextCast--;

	if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
		CastPowerSurge();

	return ChainLightningDamage * EnergyResist;
}

float Player::CastCrushingDarkness() {

	CrushingDarknessCastTime = 2 * (1 - AlacrityPercent);
	Time += CrushingDarknessCastTime;

	CrushingDarknessDamage = (1.3) * (1 + ForceCriticalChance * SurgePercent) * ((.031 * STDHP + 1.279 * BonusDamage) + (.0442 * STDHP + 1.84 * BonusDamage));
	CrushingDarknessCasts++;
	CrushingDarknessNextCast = Time + 15;

	for (short i = 0; i < 6; i++) {
		if (PolarityShift == 0 && rand() % 100 < 30)
		PolarityShiftNextCast--;
	}

	if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
		CastPowerSurge();

	return CrushingDarknessDamage * KineticResist;
}

float Player::CastLightningStrike() {

	LightningStrikeCastTime = 1.5 * (1 - AlacrityPercent);
	Time += LightningStrikeCastTime;

	if (RecklessnessCharges != 0) {
		LightningStrikeDamage = (1 + .3 * .3) * (1 + (ForceCriticalChance + .06 + .6) * (SurgePercent + .5)) * (.0375 * STDHP + 1.56 * BonusDamage);
		RecklessnessCharges--;
	}

	else
		LightningStrikeDamage = (1 + .3 * .3) * (1 + (ForceCriticalChance + .06) * (SurgePercent + .5)) * (.0375 * STDHP + 1.56 * BonusDamage);

	if (PolarityShift == 0 && rand() % 100 < 30)
		PolarityShiftNextCast--;

	else if (PolarityShift == 1 && rand() % 100 < 60)
		PolarityShiftNextCast--;

	LightningStrikeCasts++;

	if (LightningStormNextCast <= Time && rand() % 100 < 60) {
		LightningStorm = 1;
		LightningStormNextCast = Time + 10;
	}

	if (PowerSurgeNextCast <= Time && rand() % 100 < 30)
		CastPowerSurge();
		
	return LightningStrikeDamage * EnergyResist;
}

void Player::CastRecklessness() {

	RecklessnessNextCast = Time + 90;
	RecklessnessCharges = 2;
	RecklessnessCasts++;

}

void Player::CastPolarityShift() {

	PolarityShiftNextCast = Time + 90;
	PolarityShift = 1;
	PolarityShiftExpires = Time + 15;
	PolarityShiftCasts++;
	AlacrityPercent += .2;

}

void Player::CastPowerSurge() {

	PowerSurge = 1;
	PowerSurgeExpires = Time + 6;
	PowerSurgeNextCast = Time + 20;
	PowerSurgeCasts++;

	BonusDamage += 550 * .23 * UNNATURAL_MIGHT * 1.06;

}

void Player::CastPowerBoost() {

	PowerBoost = 1;
	PowerBoostExpires = Time + 30;
	PowerBoostNextCast = Time + 120;
	PowerBoostCasts++;

	BonusDamage += 425 * .23 * UNNATURAL_MIGHT * 1.06;

}

void Player::CastForceMasterAlacrity() {

	ForceMasterAlacrity = 1;
	ForceMasterAlacrityExpires = Time + 15;
	ForceMasterAlacrityNextCast = Time + 15;
	ForceMasterAlacrityCasts++;

	AlacrityPercent += .05;

}

float Player::Rotation() {

	Time = 0;
	TotalDamage = 0;

	Tick1 = 0;
	Tick2 = 0;
	Tick3 = 0;
	Tick4 = 0;
	Tick5 = 0;
	Tick6 = 0;
	AfflictionExpires = 0;
	TargetAfflictionDebuffed = 0;

	CrushingDarknessCasts = 0;
	AfflictionCasts = 0;
	ForceLightningCasts = 0;
	ChainLightningCasts = 0;
	ThunderingBlastCasts = 0;
	LightningStrikeCasts = 0;

	LightningBarrage = 0;
	LightningStorm = 0;

	CrushingDarknessNextCast = 0;
	ThunderingBlastNextCast = 0;
	LightningBarrageNextCast = 0;
	LightningStormNextCast = 0;

	RecklessnessCharges = 0;
	RecklessnessNextCast = 0;
	RecklessnessCasts = 0;

	PolarityShift = 0;
	PolarityShiftNextCast = 0;
	PolarityShiftCasts = 0;
	PolarityShiftExpires = 0;

	PowerSurge = 0;
	PowerSurgeNextCast = 0;
	PowerSurgeExpires = 0;
	PowerSurgeCasts = 0;

	PowerBoost = 0;
	PowerBoostNextCast = 0;
	PowerBoostExpires = 0;
	PowerBoostCasts = 0;

	ForceMasterAlacrity = 0;
	ForceMasterAlacrityNextCast = 0;
	ForceMasterAlacrityExpires = 0;
	ForceMasterAlacrityCasts = 0;
	
	while (Time < 1000000) {

		if (PolarityShiftExpires <= Time && PolarityShift == 1) {
			PolarityShift = 0;
			AlacrityPercent -= .2;
		}

		if (PowerSurgeExpires <= Time && PowerSurge == 1) {
			PowerSurge = 0;
			BonusDamage -= 550 * .23 * UNNATURAL_MIGHT * 1.06;
		}

		if (PowerBoostExpires <= Time && PowerBoost == 1) {
			PowerBoost = 0;
			BonusDamage -= 425 * .23 * UNNATURAL_MIGHT * 1.06;
		}

		if (ForceMasterAlacrityExpires <= Time && ForceMasterAlacrity == 1) {
			ForceMasterAlacrity = 0;
			AlacrityPercent -= .05;
		}

		if (PowerBoostNextCast <= Time)
			CastPowerBoost();

		if (RecklessnessNextCast <= Time) 
			CastRecklessness();

		if (PolarityShiftNextCast <= Time)
			CastPolarityShift();

		if (TargetAfflictionDebuffed == 1)
			AfflictionTick();

		if (LightningBarrage == 1)
			TotalDamage += CastForceLightning();
		
		else if (CrushingDarknessNextCast <= Time)
			TotalDamage += CastCrushingDarkness();

		else if (LightningStorm == 1)
			TotalDamage += CastChainLightning();

		else if (AfflictionExpires <= Time - 2)
			CastAffliction();

		else if (ThunderingBlastNextCast <= Time && RecklessnessCharges == 0)
			TotalDamage += CastThunderingBlast();

		else
			TotalDamage += CastLightningStrike();

	}

	if (PolarityShift == 1) {
		PolarityShift = 0;
		AlacrityPercent -= .2;
	}

	if (PowerSurge == 1) {
		PowerSurge = 0;
		BonusDamage -= 550 *.23 * UNNATURAL_MIGHT * 1.06;
	}

	if (PowerBoost == 1) {
		PowerBoost = 0;
		BonusDamage -= 425 * .23 * UNNATURAL_MIGHT * 1.06;
	}

	if (ForceMasterAlacrity == 1) {
		ForceMasterAlacrity = 0;
		AlacrityPercent -= .05;
	}

	return TotalDamage / Time;
}

void Player::DisplayRotationStats() {

	cout << "\n\nRotation Time: " << Time << " seconds\n";
	cout << "Total Damage: " << TotalDamage << "\n";
	cout << "DPS: " << TotalDamage / Time << " against an OPs boss with 30% Eres, 35% Kres and 0% Ires\n\n";

	cout << "Crushing Darkness casts: " << CrushingDarknessCasts << "\n";
	cout << "Affliction casts: " << AfflictionCasts << "\n";
	cout << "Force Lightning casts: " << ForceLightningCasts << "\n";
	cout << "Chain Lightning casts: " << ChainLightningCasts << "\n";
	cout << "Thundering Blast casts: " << ThunderingBlastCasts << "\n";
	cout << "Lightning Strike casts: " << LightningStrikeCasts << "\n\n";

	cout << "Power Surge casts: " << PowerSurgeCasts << "\n";
	cout << "Power Boost casts: " << PowerBoostCasts << "\n";
	cout << "Force-Master Alacrity casts (4-pc.): " << ForceMasterAlacrityCasts << "\n";
	cout << "Recklessness casts: " << RecklessnessCasts << "\n";
	cout << "Polarity Shift casts: " << PolarityShiftCasts << "\n\n";

}

float Player::modOffStats (string STAT1_NAME, float STAT1, string STAT2_NAME, float STAT2, string STAT3_NAME, float STAT3, string STAT4_NAME, float STAT4,
	string STAT5_NAME, float STAT5, string STAT6_NAME, float STAT6, short PROCESS) {

	//STAT1 Accuracy, STAT2 Willpower, STAT3 Power, STAT4 Crit, STAT5 Surge, STAT6 Alacrity
	string stat_name[6] = {STAT1_NAME, STAT2_NAME, STAT3_NAME, STAT4_NAME, STAT5_NAME, STAT6_NAME};
	double rating[6] = {STAT1, STAT2, STAT3, STAT4, STAT5, STAT6};

	for (short i = 0; i < 6; i++) {

		if (stat_name[i] == "AC") {
			AccuracyRating += rating[i];
			AccuracyFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , AccuracyRating / 55.0 / 1.2 ));
			AccuracyPercent = AccuracyFromRating + AccuracyFromSkillsBuffs;
		}

		else if (stat_name[i] == "WP") {
			WillpowerRating += rating[i];
			TotalWillpower = WillpowerRating * (1 + MARK_OF_POWER_STAT + .06);
			CritFromWillpower = .2 * ( 1.0 - pow( 1.0 - (.01 / .2) , TotalWillpower / 55.0 / 5.5 ));
			ForceCriticalChance = CritFromRating + CritFromWillpower + CritFromSkillsBuffs + BaseCriticalChance;
			BonusDamageFromWillpower = TotalWillpower * .2;
			BonusDamage = (BonusDamageFromWillpower + BonusDamageFromPower + BonusDamageFromForcePower) * UNNATURAL_MIGHT * 1.06;
		}

		else if (stat_name[i] == "PO") {
			PowerRating += rating[i];
			BonusDamageFromPower = PowerRating * .23;
			BonusDamage = (BonusDamageFromWillpower + BonusDamageFromPower + BonusDamageFromForcePower) * UNNATURAL_MIGHT * 1.06;
		}

		else if (stat_name[i] == "CR") {
			CritRating += rating[i];
			CritFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , CritRating / 55.0 / .9 ));
			ForceCriticalChance = CritFromRating + CritFromWillpower + CritFromSkillsBuffs + BaseCriticalChance;
		}

		else if (stat_name[i] == "SU") {
			SurgeRating += rating[i];
			SurgeFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , SurgeRating / 55.0 / .22 ));
			SurgePercent = SurgeFromRating + SurgeFromSkillsBuffs + BaseSurge;
		}

		else if (stat_name[i] == "AL") {
			AlacrityRating += rating[i];
			AlacrityFromRating = .3 * ( 1.0 - pow( 1.0 - (.01 / .3) , AlacrityRating / 55.0 / 1.25 ));
			AlacrityPercent = BaseAlacrity + AlacrityFromSkillsBuffs + AlacrityFromRating;
		}

	}
	
	if (PROCESS == 1 && AccuracyRating < 434)
		return Rotation() * (AccuracyPercent / 1.1);

	else if (PROCESS == 1 && AccuracyRating >= 434)
		return Rotation() * 1.1;

	else
		return 0;
}

void Player::showOffStats () {

	cout << "\n\nAccuracy Rating : " << AccuracyRating << "\n";
	cout << "Accuracy %: " << AccuracyPercent << "\n\n";

	cout << "Willpower: " << TotalWillpower << "\n";
	cout << "Power: " << PowerRating << "\n";
	cout << "ForcePower: " << ForcePower << "\n";
	cout << "Bonus Damage: " << BonusDamage << "\n\n";

	cout << "Crit Rating: " << CritRating << "\n";
	cout << "ForceCrit %: " << ForceCriticalChance << "\n\n";

	cout << "SurgeRating: " << SurgeRating << "\n";
	cout << "Surge %: " << SurgePercent << "\n\n";

	cout << "AlacrityRating: " << AlacrityRating << "\n";
	cout << "Alacrity %: " << AlacrityPercent << "\n\n";

}

void Player::OffHiltArmorings(float StatCompare[]){

	for (short i = 0; i < 9; i++) {
		//Advanced Resolve Armoring/Hilt 31
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 88.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -88.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Force Wielder Armoring/Hilt 31
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 79.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -79.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		if (StatCompare[0] > StatCompare[1]) {
			modOffStats("AC", 0.0, "WP", 88.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Resolve 31 is greater.";
		}

		else if (StatCompare[1] > StatCompare[0]) {
			modOffStats("AC", 0.0, "WP", 79.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Force Wielder 31 is greater.";
		}
	}

}

void Player::OffMods(float StatCompare[]) {

	for (short i = 0; i < 9; i++) {
		//Advanced Aptitude Mod 31
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 69.0, "PO", 57.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -69.0, "PO", -57.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Aptitude Mod 31A
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 84.0, "PO", 32.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -84.0, "PO", -32.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Aptitude Mod 31B
		StatCompare[2] = modOffStats("AC", 0.0, "WP", 57.0, "PO", 46.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -57.0, "PO", -46.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Mettle Mod 31
		StatCompare[3] = modOffStats("AC", 0.0, "WP", 69.0, "PO", 0.0, "CR", 57.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -69.0, "PO", 0.0, "CR", -57.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Mettle Mod 31A
		StatCompare[4] = modOffStats("AC", 0.0, "WP", 84.0, "PO", 0.0, "CR", 32.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -84.0, "PO", 0.0, "CR", -32.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Mettle Mod 31B
		StatCompare[5] = modOffStats("AC", 0.0, "WP", 57.0, "PO", 0.0, "CR", 46.0, "SU", 0.0, "AL", 0.0, 1);
					   	 modOffStats("AC", 0.0, "WP", -57.0, "PO", 0.0, "CR", -46.0, "SU", 0.0, "AL", 0.0, 0);

		if (StatCompare[0] > StatCompare[1] && 
			StatCompare[0] > StatCompare[2] && 
			StatCompare[0] > StatCompare[3] &&
			StatCompare[0] > StatCompare[4] && 
			StatCompare[0] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 69.0, "PO", 57.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Aptitude Mod 31 is greater.";
		}

		else if (StatCompare[1] > StatCompare[0] && 
			     StatCompare[1] > StatCompare[2] && 
				 StatCompare[1] > StatCompare[3] && 
				 StatCompare[1] > StatCompare[4] && 
				 StatCompare[1] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 84.0, "PO", 32.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Aptitude Mod 31A is greater.";
		}

		else if (StatCompare[2] > StatCompare[0] && 
				 StatCompare[2] > StatCompare[1] && 
				 StatCompare[2] > StatCompare[3] && 
				 StatCompare[2] > StatCompare[4] && 
				 StatCompare[2] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 57.0, "PO", 46.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Aptitude Mod 31B is greater.";
		}

		else if (StatCompare[3] > StatCompare[0] && 
				 StatCompare[3] > StatCompare[1] && 
				 StatCompare[3] > StatCompare[2] && 
				 StatCompare[3] > StatCompare[4] && 
				 StatCompare[3] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 69.0, "PO", 0.0, "CR", 57.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Mettle Mod 31 is greater.";
		}

		else if (StatCompare[4] > StatCompare[0] && 
				 StatCompare[4] > StatCompare[1] && 
				 StatCompare[4] > StatCompare[2] && 
				 StatCompare[4] > StatCompare[3] && 
				 StatCompare[4] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 84.0, "PO", 0.0, "CR", 32.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Mettle Mod 31A is greater.";
		}

		else if (StatCompare[5] > StatCompare[0] && 
				 StatCompare[5] > StatCompare[1] && 
				 StatCompare[5] > StatCompare[2] && 
				 StatCompare[5] > StatCompare[3] && 
				 StatCompare[5] > StatCompare[4]) {
			modOffStats("AC", 0.0, "WP", 57.0, "PO", 0.0, "CR", 46.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Mettle Mod 31B is greater.";
		}
	}

}

void Player::OffEnhancements(float StatCompare[]) {

	for (short i = 0; i < 7; i++) {
		//Adv Adept 31
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 52.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", -52.0, "CR", 0.0, "SU", -79.0, "AL", 0.0, 0);

		//Adv Quick Savant 31
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 52.0, "CR", 0.0, "SU", 0.0, "AL", 79.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", -52.0, "CR", 0.0, "SU", 0.0, "AL", -79.0, 0);

		//Adv Battle 31
		StatCompare[2] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 52.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", -52.0, "SU", -79.0, "AL", 0.0, 0);

		//Adv Insight 31
		StatCompare[3] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 52.0, "SU", 0.0, "AL", 79.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", -52.0, "SU", 0.0, "AL", -79.0, 0);

		//Adv Acute 31
		StatCompare[4] = modOffStats("AC", 79.0, "WP", 0.0, "PO", 0.0, "CR", 52.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", -79.0, "WP", 0.0, "PO", 0.0, "CR", -52.0, "SU", 0.0, "AL", 0.0, 0);

		//Adv Initiative 31
		StatCompare[5] = modOffStats("AC", 79.0, "WP", 0.0, "PO", 52.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", -79.0, "WP", 0.0, "PO", -52.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		if (StatCompare[0] > StatCompare[1] && 
			StatCompare[0] > StatCompare[2] && 
			StatCompare[0] > StatCompare[3] && 
			StatCompare[0] > StatCompare[4] && 
			StatCompare[0] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 52.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nAdv. Adept 31 is greater.";
		}

		else if (StatCompare[1] > StatCompare[0] && 
				 StatCompare[1] > StatCompare[2] && 
				 StatCompare[1] > StatCompare[3] && 
				 StatCompare[1] > StatCompare[4] && 
				 StatCompare[1] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 52.0, "CR", 0.0, "SU", 0.0, "AL", 79.0 ,0);
			cout << "\nAdv. Quick Savant 31 is greater.";
		}

		else if (StatCompare[2] > StatCompare[0] && 
				 StatCompare[2] > StatCompare[1] && 
				 StatCompare[2] > StatCompare[3] && 
				 StatCompare[2] > StatCompare[4] && 
				 StatCompare[2] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 52.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nAdv. Battle 31 is greater.";
		}

		else if (StatCompare[3] > StatCompare[0] && 
				 StatCompare[3] > StatCompare[1] && 
				 StatCompare[3] > StatCompare[2] && 
				 StatCompare[3] > StatCompare[4] && 
				 StatCompare[3] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 52.0, "SU", 0.0, "AL", 79.0, 0);
			cout << "\nAdv. Insight 31 is greater.";
		}

		else if (StatCompare[4] > StatCompare[0] && 
				 StatCompare[4] > StatCompare[1] && 
				 StatCompare[4] > StatCompare[2] && 
				 StatCompare[4] > StatCompare[3] && 
				 StatCompare[4] > StatCompare[5]) {
			modOffStats("AC", 79.0, "WP", 0.0, "PO", 0.0, "CR", 52.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Acute 31 is greater.";
		}

		else if (StatCompare[5] > StatCompare[0] && 
				 StatCompare[5] > StatCompare[1] && 
				 StatCompare[5] > StatCompare[2] && 
				 StatCompare[5] > StatCompare[3] && 
				 StatCompare[5] > StatCompare[4]) {
			modOffStats("AC", 79.0, "WP", 0.0, "PO", 52.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Initiative 31 is greater.";
		}

	}
	
}

void Player::OffEarpieces(float StatCompare[]) {
	
		//UW Force-Master's Device
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 0.0, "AL", 79.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", 0.0, "CR", -110.0, "SU", 0.0, "AL", -79.0, 0);

		//UW Force-Mystic's Device
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 110.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", -110.0, "CR", 0.0, "SU", -79.0, "AL", 0.0, 0);

		//UW Stalker's Device
		StatCompare[2] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", 0.0, "CR", -110.0, "SU", -79.0, "AL", 0.0, 0);

		//Verpine Force-Lord's MK-1 Relay
		StatCompare[3] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 88.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", 0.0, "CR", -88.0, "SU", -79.0, "AL", 0.0, 0);

		//Verpine Force-Lord's MK-2 Relay
		StatCompare[4] = modOffStats("AC", 79.0, "WP", 172.0, "PO", 84.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", -79.0, "WP", -172.0, "PO", -84.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Verpine Force-Healer's MK-1 Chip
		StatCompare[5] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 88.0, "CR", 0.0, "SU", 0.0, "AL", 79.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", -88.0, "CR", 0.0, "SU", 0.0, "AL", -79.0, 0);

		//Verpine Force-Healer's MK-2 Chip
		StatCompare[6] = modOffStats("AC", 0.0, "WP", 172.0, "PO", 0.0, "CR", 84.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -172.0, "PO", 0.0, "CR", -84.0, "SU", -79.0, "AL", 0.0, 0);

		if (StatCompare[0] > StatCompare[1] &&
			StatCompare[0] > StatCompare[2] &&
			StatCompare[0] > StatCompare[3] &&
			StatCompare[0] > StatCompare[4] &&
			StatCompare[0] > StatCompare[5] &&
			StatCompare[0] > StatCompare[6] &&
			StatCompare[0] > StatCompare[7]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 0.0, "AL", 79.0, 0);
			cout << "\nUnderworld Force-Master's Device is greater.";
		}

		else if (StatCompare[1] > StatCompare[0] &&
				 StatCompare[1] > StatCompare[2] &&
				 StatCompare[1] > StatCompare[3] &&
				 StatCompare[1] > StatCompare[4] &&
				 StatCompare[1] > StatCompare[5] &&
				 StatCompare[1] > StatCompare[6]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 110.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nUnderworld Force-Mystic's Device is greater.";
		}

		else if (StatCompare[2] > StatCompare[1] &&
				 StatCompare[2] > StatCompare[0] &&
				 StatCompare[2] > StatCompare[3] &&
				 StatCompare[2] > StatCompare[4] &&
				 StatCompare[2] > StatCompare[5] &&
				 StatCompare[2] > StatCompare[6]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nUnderworld Stalker's Device is greater.";
		}

		else if (StatCompare[3] > StatCompare[1] &&
				 StatCompare[3] > StatCompare[2] &&
				 StatCompare[3] > StatCompare[0] &&
				 StatCompare[3] > StatCompare[4] &&
				 StatCompare[3] > StatCompare[5] &&
				 StatCompare[3] > StatCompare[6]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 88.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Lord's MK-1 Relay is greater.";
		}

		else if (StatCompare[4] > StatCompare[1] &&
				 StatCompare[4] > StatCompare[2] &&
				 StatCompare[4] > StatCompare[3] &&
				 StatCompare[4] > StatCompare[0] &&
				 StatCompare[4] > StatCompare[5] &&
				 StatCompare[4] > StatCompare[6]) {
			modOffStats("AC", 79.0, "WP", 172.0, "PO", 84.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Lord's MK-2 Relay is greater.";
		}

		else if (StatCompare[5] > StatCompare[1] &&
				 StatCompare[5] > StatCompare[2] &&
				 StatCompare[5] > StatCompare[3] &&
				 StatCompare[5] > StatCompare[4] &&
				 StatCompare[5] > StatCompare[0] &&
				 StatCompare[5] > StatCompare[6]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 88.0, "CR", 0.0, "SU", 0.0, "AL", 79.0, 0);
			cout << "\nVerpine Force-Healer's MK-1 Chip is greater.";
		}

		else if (StatCompare[6] > StatCompare[1] &&
				 StatCompare[6] > StatCompare[2] &&
				 StatCompare[6] > StatCompare[3] &&
				 StatCompare[6] > StatCompare[4] &&
				 StatCompare[6] > StatCompare[5] &&
				 StatCompare[6] > StatCompare[0]) {
			modOffStats("AC", 0.0, "WP", 172.0, "PO", 0.0, "CR", 84.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Healer's MK-2 Chip is greater.";
		}

}

void Player::OffImplants(float StatCompare[]) {

	for (short i = 0; i < 2; i++) {
		//UW Force-Master/Stalker's MK-V Package
		StatCompare[0] = modOffStats("AC", 79.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", -79.0, "WP", -159.0, "PO", 0.0, "CR", -110.0, "SU", 0.0, "AL", 0.0, 0);

		//UW Force-Master/Mystic's MK-X Package
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", 0.0, "CR", -110.0, "SU", -79.0, "AL", 0.0, 0);

		//UW Force-Mystic's MK-V Package
		StatCompare[2] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 110.0, "CR", 0.0, "SU", 0.0, "AL", 79.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", -110.0, "CR", 0.0, "SU", 0.0, "AL", -79.0, 0);

		//UW Stalker's MK-X Package
		StatCompare[3] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 110.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", -110.0, "CR", 0.0, "SU", -79.0, "AL", 0.0, 0);

		//Verpine Force-Lord's MK-1 System
		StatCompare[4] = modOffStats("AC", 79.0, "WP", 159.0, "PO", 0.0, "CR", 88.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", -79.0, "WP", -159.0, "PO", 0.0, "CR", -88.0, "SU", 0.0, "AL", 0.0, 0);

		//Verpine Force-Lord's MK-2 System
		StatCompare[5] = modOffStats("AC", 0.0, "WP", 172.0, "PO", 0.0, "CR", 84.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -172.0, "PO", 0.0, "CR", -84.0, "SU", -79.0, "AL", 0.0, 0);

		//Verpine Force-Healer's MK-1 Motivator
		StatCompare[6] = modOffStats("AC", 0.0, "WP", 159.0, "PO", 88.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -159.0, "PO", -88.0, "CR", 0.0, "SU", -79.0, "AL", 0.0, 0);

		//Verpine Force-Healer's MK-2 Motivator
		StatCompare[7] = modOffStats("AC", 0.0, "WP", 172.0, "PO", 84.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -172.0, "PO", -84.0, "CR", 0.0, "SU", -79.0, "AL", 0.0, 0);

		if (StatCompare[0] > StatCompare[1] &&
			StatCompare[0] > StatCompare[2] &&
			StatCompare[0] > StatCompare[3] &&
			StatCompare[0] > StatCompare[4] &&
			StatCompare[0] > StatCompare[5] &&
			StatCompare[0] > StatCompare[6] &&
			StatCompare[0] > StatCompare[7]) {
			modOffStats("AC", 79.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nUW Force-Master/Stalker's MK-V Package is greater.";
		}

		else if (StatCompare[1] > StatCompare[0] &&
				 StatCompare[1] > StatCompare[2] &&
				 StatCompare[1] > StatCompare[3] &&
				 StatCompare[1] > StatCompare[4] &&
				 StatCompare[1] > StatCompare[5] &&
				 StatCompare[1] > StatCompare[6] &&
				 StatCompare[1] > StatCompare[7]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 0.0, "CR", 110.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nUW Force-Master/Mystic's MK-X Package is greater.";
		}

		else if (StatCompare[2] > StatCompare[1] &&
				 StatCompare[2] > StatCompare[0] &&
				 StatCompare[2] > StatCompare[3] &&
				 StatCompare[2] > StatCompare[4] &&
				 StatCompare[2] > StatCompare[5] &&
				 StatCompare[2] > StatCompare[6] &&
				 StatCompare[2] > StatCompare[7]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 110.0, "CR", 0.0, "SU", 0.0, "AL", 79.0, 0);
			cout << "\nUW Force-Mystic's MK-V Package is greater.";
		}

		else if (StatCompare[3] > StatCompare[1] &&
				 StatCompare[3] > StatCompare[2] &&
				 StatCompare[3] > StatCompare[0] &&
				 StatCompare[3] > StatCompare[4] &&
				 StatCompare[3] > StatCompare[5] &&
				 StatCompare[3] > StatCompare[6] &&
				 StatCompare[3] > StatCompare[7]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 110.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nUW Stalker's MK-X Package is greater.";
		}

		else if (StatCompare[4] > StatCompare[1] &&
				 StatCompare[4] > StatCompare[2] &&
				 StatCompare[4] > StatCompare[3] &&
				 StatCompare[4] > StatCompare[0] &&
				 StatCompare[4] > StatCompare[5] &&
				 StatCompare[4] > StatCompare[6] &&
				 StatCompare[4] > StatCompare[7]) {
			modOffStats("AC", 79.0, "WP", 159.0, "PO", 0.0, "CR", 88.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Lord's MK-1 System is greater.";
		}

		else if (StatCompare[5] > StatCompare[1] &&
				 StatCompare[5] > StatCompare[2] &&
				 StatCompare[5] > StatCompare[3] &&
				 StatCompare[5] > StatCompare[4] &&
				 StatCompare[5] > StatCompare[0] &&
				 StatCompare[5] > StatCompare[6] &&
				 StatCompare[5] > StatCompare[7]) {
			modOffStats("AC", 0.0, "WP", 172.0, "PO", 0.0, "CR", 84.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Lord's MK-2 System is greater.";
		}

		else if (StatCompare[6] > StatCompare[1] &&
				 StatCompare[6] > StatCompare[2] &&
				 StatCompare[6] > StatCompare[3] &&
				 StatCompare[6] > StatCompare[4] &&
				 StatCompare[6] > StatCompare[5] &&
				 StatCompare[6] > StatCompare[0] &&
				 StatCompare[6] > StatCompare[7]) {
			modOffStats("AC", 0.0, "WP", 159.0, "PO", 88.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Healer's MK-1 Motivator is greater.";
		}

		else if (StatCompare[7] > StatCompare[1] &&
				 StatCompare[7] > StatCompare[2] &&
				 StatCompare[7] > StatCompare[3] &&
				 StatCompare[7] > StatCompare[4] &&
				 StatCompare[7] > StatCompare[5] &&
				 StatCompare[7] > StatCompare[6] &&
				 StatCompare[7] > StatCompare[0]) {
			modOffStats("AC", 0.0, "WP", 172.0, "PO", 84.0, "CR", 0.0, "SU", 79.0, "AL", 0.0, 0);
			cout << "\nVerpine Force-Healer's MK-2 Motivator is greater.";
		}

	}

}

void Player::OffColorCrystals (float StatCompare[]) {

	for (short i = 0; i < 2; i++) {
		//Advanced Hawkeye
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 41.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", -41.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Eviscerating
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 41.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", -41.0, "SU", 0.0, "AL", 0.0, 0);

		if (StatCompare[0] > StatCompare[1]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 41.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Hawkeye Color Crystal is greater.";
		}

		else if (StatCompare[1] > StatCompare[0]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 41.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv. Eviscerating Color Crystal is greater.";
		}
	}

}

void Player::OffAugments (float StatCompare[]) { 

	for (short i = 0; i < 14; i++) {
		//Advanced Critical Augment 28
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 32.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", -32.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Overkill Augment 28
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 32.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", -32.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Advanced Surge Augment 28
		StatCompare[2] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 32.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", -32.0, "AL", 0.0, 0);

		//Advanced Alacrity Augment 28
		StatCompare[3] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 32.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", -32.0, 0);

		//Advanced Resolve Augment 28
		StatCompare[4] = modOffStats("AC", 0.0, "WP", 32.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", -32.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
		
		if (AccuracyRating < 434) {
		//Advanced Accuracy Augment 28
		StatCompare[5] = modOffStats("AC", 32.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", -32.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
		}

		else
			StatCompare[5] = 0;

		if (StatCompare[0] > StatCompare[1] && 
			StatCompare[0] > StatCompare[2] && 
			StatCompare[0] > StatCompare[3] &&
			StatCompare[0] > StatCompare[4] &&
			StatCompare[0] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 32.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv Critical Augment 28 is greater.";
		}

		else if (StatCompare[1] > StatCompare[0] && 
			     StatCompare[1] > StatCompare[2] && 
				 StatCompare[1] > StatCompare[3] && 
				 StatCompare[1] > StatCompare[4] &&
				 StatCompare[1] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 32.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv Overkill Augment 28 is greater.";
		}

		else if (StatCompare[2] > StatCompare[0] && 
				 StatCompare[2] > StatCompare[1] && 
				 StatCompare[2] > StatCompare[3] && 
				 StatCompare[2] > StatCompare[4] &&
				 StatCompare[2] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 32.0, "AL", 0.0, 0);
			cout << "\nAdv Surge Augment 28 is greater.";
		}

		else if (StatCompare[3] > StatCompare[0] && 
				 StatCompare[3] > StatCompare[1] && 
				 StatCompare[3] > StatCompare[2] && 
				 StatCompare[3] > StatCompare[4] &&
				 StatCompare[3] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 32.0, 0);
			cout << "\nAdv Alacrity Augment 28 is greater.";
		}

		else if (StatCompare[4] > StatCompare[0] && 
				 StatCompare[4] > StatCompare[1] && 
				 StatCompare[4] > StatCompare[2] && 
				 StatCompare[4] > StatCompare[3] &&
				 StatCompare[4] > StatCompare[5]) {
			modOffStats("AC", 0.0, "WP", 32.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv Resolve Augment 28 is greater.";
		}

		else if (StatCompare[5] > StatCompare[0] && 
				 StatCompare[5] > StatCompare[1] && 
				 StatCompare[5] > StatCompare[2] && 
				 StatCompare[5] > StatCompare[3] &&
				 StatCompare[5] > StatCompare[4]) {
			modOffStats("AC", 32.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nAdv Accuracy Augment 28 is greater.";
		}

	}

}

void Player::Template(float StatCompare[]){

	for (short i = 0; i < 9; i++) {
		//Item1
		StatCompare[0] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item2
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item3
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item4
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item5
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item6
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item7
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item8
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item9
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item10
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
		
		//Item11
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);

		//Item12
		StatCompare[1] = modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 1);
						 modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);



		if (StatCompare[0] > StatCompare[1] &&
			StatCompare[0] > StatCompare[2] &&
			StatCompare[0] > StatCompare[3] &&
			StatCompare[0] > StatCompare[4] &&
			StatCompare[0] > StatCompare[5] &&
			StatCompare[0] > StatCompare[6] &&
			StatCompare[0] > StatCompare[7] &&
			StatCompare[0] > StatCompare[7] &&
			StatCompare[0] > StatCompare[8] &&
			StatCompare[0] > StatCompare[9] &&
			StatCompare[0] > StatCompare[10] &&
			StatCompare[0] > StatCompare[11]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nITEM1 is greater.";
		}

		else if (StatCompare[0] > StatCompare[1] &&
				 StatCompare[0] > StatCompare[2] &&
				 StatCompare[0] > StatCompare[3] &&
				 StatCompare[0] > StatCompare[4] &&
				 StatCompare[0] > StatCompare[5] &&
				 StatCompare[0] > StatCompare[6] &&
				 StatCompare[0] > StatCompare[7] &&
				 StatCompare[0] > StatCompare[7] &&
				 StatCompare[0] > StatCompare[8] &&
				 StatCompare[0] > StatCompare[9] &&
				 StatCompare[0] > StatCompare[10] &&
				 StatCompare[0] > StatCompare[11]) {
			modOffStats("AC", 0.0, "WP", 0.0, "PO", 0.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
			cout << "\nITEM2 is greater.";
		}
	}

}
