/* Hardpoint.h
Copyright (c) 2016 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef HARDPOINT_H_
#define HARDPOINT_H_

#include "Angle.h"
#include "Point.h"

#include <vector>

class Outfit;
class Projectile;
class Ship;
class Visual;



// A single weapon hardpoint on the ship (i.e. a gun port or turret mount),
// which may or may not have a weapon installed.
class Hardpoint {
public:
	// Tracking mode (for turrets).
	// TODO: Just do a PR with only tracking (no Triggering and FireGroups)
	enum Tracking {FOCUSSED, OPPORTUNISTIC};
	
	// Triggering mode (for guns and turrets)
	// - Manual: The weapon fires whenever the manual trigger is active.
	// - Gated: The weapon fires whenever the manual trigger is active and it can hit any enemy.
	// - Auto: The weapon fires whenever it can hit any enemy or when the manual trigger is active.
	enum Triggering {MANUAL, GATED, AUTO};
	
	// Combinations:
	// ------------- | ---------- | ------------- |
	// Tracking      | Triggering | Combo name    | Combo art
	// ------------- | ---------- | ------------- |
	// FOCUSSED      | MANUAL     | FOCUSSED      | Scope sight
	// FOCUSSED      | GATED      | GATED         | Scope sight with target
	// FOCUSSED      | AUTO       | DIRECTED      | Scope sight with target and chip housing
	// OPPORTUNISTIC | MANUAL     | OPPORTUNISTIC | 'Waaier'
	// OPPORTUNISTIC | GATED      | SEMI-AUTO     | 'Waaier with target'
	// OPPORTUNISTIC | AUTO       | FULL-AUTO     | 'Waaier with target and chip housing'
	
	
	// Fire triggering mode (for all weapon types), determines tracking and when the weapon in the hardpoint fires.
	// - Triggered; tracks the pilots selected target and fires when relevant trigger is active.
	// - Focussed; tracks the pilots main target and fires when the pilot presses the trigger.
	// - Gated; focusses on nearby targets and fires when relevant trigger is active and it has a possibility to hit any target.
	// - Directed; tracks the pilots selected target and fires when it has the possiblity to hit any hostile target.
	// - Opportunistic; tracks targets independently and fires when it has the possibility to hit any hostile target.
	
	// Manual  | Target       | Target      | Mode name
	// gating  | tracking     | gating      | 
	// ------- |---------     |------------ | -------------
	// trigger | selected     | no          | TRIGGERED
	// trigger | selected     | any         | FOCUSSED
	// trigger | any          | any         | GATED
	// no      | selected     | any         | DIRECTED
	// no      | any          | any         | OPPORTUNISTIC
	
	enum Triggering {TRIGGERED, FOCUSSED, GATED, DIRECTED, OPPORTUNISTIC};
	
	// Triggering group, bitmask that describes the various groups.
	// - All weapons in group 0 are triggered by the primary firing key.
	// - All weapons in group 1 and higher are triggered by the secondary firing key.
	// - The secondary select key selects which of the secondary groups is triggered by the secondary firing key.
	// TODO: move triggering group to UI screen (and per weapon type instead of per hardpoint)
	uint64_t GetTriggerGroups();
	void AddTriggerGroups(uint64_t);
	void RemoveTriggerGroups(uint64_t);
	
	// Get and set the triggering mode.
	Triggering GetTriggering() const;
	void SetTriggering(Triggering mode);
	
	// Get and set the tracking mode (for turret hardpoints).
	Tracking GetTracking() const;
	void SetTracking(Tracking mode);


public:
	// Constructor. Hardpoints may or may not specify what weapon is in them.
	Hardpoint(const Point &point, const Angle &baseAngle, bool isTurret, bool isParallel, const Outfit *outfit = nullptr);
	
	// Get the weapon installed in this hardpoint (or null if there is none).
	const Outfit *GetOutfit() const;
	// Get the location, relative to the center of the ship, from which
	// projectiles of this weapon should originate. This point must be
	// rotated to take the ship's current facing direction into account.
	const Point &GetPoint() const;
	// Get the angle that this weapon is aimed at, relative to the ship.
	const Angle &GetAngle() const;
	// Get the base angle that this weapon is aimed at (without harmonization/convergence), relative to the ship.
	const Angle &GetBaseAngle() const;
	// Get the angle this weapon ought to point at for ideal gun harmonization.
	Angle HarmonizedAngle() const;
	// Shortcuts for querying weapon characteristics.
	bool IsParallel() const;
	bool IsTurret() const;
	bool IsHoming() const;
	bool IsAntiMissile() const;
	bool CanAim() const;
	
	// Check if this weapon is ready to fire.
	bool IsReady() const;
	// Check if this weapon was firing in the previous step.
	bool WasFiring() const;
	// If this is a burst weapon, get the number of shots left in the burst.
	int BurstRemaining() const;
	// Perform one step (i.e. decrement the reload count).
	void Step();
	
	// Adjust this weapon's aim by the given amount, relative to its maximum
	// "turret turn" rate.
	void Aim(double amount);
	// Fire this weapon. If it is a turret, it automatically points toward
	// the given ship's target. If the weapon requires ammunition, it will
	// be subtracted from the given ship.
	void Fire(Ship &ship, std::vector<Projectile> &projectiles, std::vector<Visual> &visuals);
	// Fire an anti-missile. Returns true if the missile should be killed.
	bool FireAntiMissile(Ship &ship, const Projectile &projectile, std::vector<Visual> &visuals);
	
	// Install a weapon here (assuming it is empty). This is only for
	// Armament to call internally.
	void Install(const Outfit *outfit);
	// Reload this weapon.
	void Reload();
	// Uninstall the outfit from this port (if it has one).
	void Uninstall();
	
	
private:
	// Reset the reload counters and expend ammunition, if any.
	void Fire(Ship &ship, const Point &start, const Angle &aim);
	
	
private:
	// The weapon installed in this hardpoint.
	const Outfit *outfit = nullptr;
	// Hardpoint location, in world coordinates relative to the ship's center.
	Point point;
	// Angle of firing direction (guns only).
	Angle baseAngle;
	// This hardpoint is for a turret or a gun.
	bool isTurret = false;
	// Indicates if this hardpoint disallows converging (guns only).
	bool isParallel = false;
	
	// Tracking and triggering mode for the weapon in this hardpoint.
	Tracking tracking = Tracking::FOCUSSED;
	Triggering triggering = Triggering::DIRECT;
	// Bitmask of the groups that trigger the weapon in this hardpoint.
	uint64_t triggerGroups;
	
	// Angle adjustment for convergence.
	Angle angle;
	// Reload timers and other attributes.
	double reload = 0.;
	double burstReload = 0.;
	int burstCount = 0;
	bool isFiring = false;
	bool wasFiring = false;
};



#endif
