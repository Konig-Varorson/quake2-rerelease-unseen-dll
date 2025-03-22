// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

assassin

==============================================================================
*/

#include "g_local.h"
#include "m_assassin.h"
#include "m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_die;
static cached_soundindex sound_sight;
static cached_soundindex sound_idle;

bool assassin_do_pounce(edict_t *self, const vec3_t &dest);
void assassin_walk(edict_t *self);
void assassin_dodge_jump(edict_t *self);
void assassin_jump_straightup(edict_t *self);
void assassin_jump_wait_land(edict_t *self);
//void assassin_false_death(edict_t *self); //KONIG - save for Ionized, no good animations
//void assassin_false_death_start(edict_t *self);
void assassin_stand(edict_t *self);

//=========================
//=========================

//=========================
//=========================
MONSTERINFO_SIGHT(assassin_sight) (edict_t *self, edict_t *other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

// ******************
// IDLE
// ******************

void assassin_idle_noise(edict_t *self)
{
	gi.sound(self, CHAN_VOICE, sound_idle, 0.5, ATTN_IDLE, 0);
}

mframe_t assassin_frames_idle[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(assassin_move_idle) = { FRAME_stand1, FRAME_stand40, assassin_frames_idle, assassin_stand };

mframe_t assassin_frames_idle2[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(assassin_move_idle2) = { FRAME_recin1, FRAME_recin40, assassin_frames_idle2, assassin_stand };

MONSTERINFO_IDLE(assassin_idle) (edict_t *self) -> void
{
	if (frandom() < 0.35f)
		M_SetAnimation(self, &assassin_move_idle);
	else
		M_SetAnimation(self, &assassin_move_idle2);
}

// ******************
// STAND
// ******************

mframe_t assassin_frames_stand[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(assassin_move_stand) = { FRAME_recin1, FRAME_recin40, assassin_frames_stand, assassin_stand };

MONSTERINFO_STAND(assassin_stand) (edict_t *self) -> void
{
	if (frandom() < 0.25f)
		M_SetAnimation(self, &assassin_move_stand);
	else
		M_SetAnimation(self, &assassin_move_idle);
}

// ******************
// RUN
// ******************

mframe_t assassin_frames_run[] = {
	{ ai_run, 13, monster_footstep },
	{ ai_run, 17 },
	{ ai_run, 21 },
	{ ai_run, 18, monster_footstep },
	{ ai_run, 17 },
	{ ai_run, 21 }
};
MMOVE_T(assassin_move_run) = { FRAME_walk1, FRAME_walk6, assassin_frames_run, nullptr };

MONSTERINFO_RUN(assassin_run) (edict_t *self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &assassin_move_stand);
	else
		M_SetAnimation(self, &assassin_move_run);
}

// ******************
// WALK
// ******************

mframe_t assassin_frames_walk[] = {
	{ ai_walk, 4, monster_footstep },
	{ ai_walk, 6 },
	{ ai_walk, 8 },
	{ ai_walk, 5, monster_footstep },
	{ ai_walk, 4 },
	{ ai_walk, 6 }
};
MMOVE_T(assassin_move_walk) = { FRAME_walk1, FRAME_walk6, assassin_frames_walk, assassin_walk };

MONSTERINFO_WALK(assassin_walk) (edict_t *self) -> void
{
	M_SetAnimation(self, &assassin_move_walk);
}

// ******************
// false death
// ******************
#if 0 //KONIG - I'll save this for Ionized
mframe_t assassin_frames_reactivate[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_false_death_end) = { FRAME_death5, FRAME_death1, assassin_frames_reactivate, assassin_run };

void assassin_reactivate(edict_t *self)
{
	self->monsterinfo.aiflags &= ~AI_STAND_GROUND;
	M_SetAnimation(self, &assassin_move_false_death_end);
}

void assassin_heal(edict_t *self)
{
	if (skill->integer == 2)
		self->health += 2;
	else if (skill->integer == 3)
		self->health += 3;
	else
		self->health++;

	self->monsterinfo.setskin(self);

	if (self->health >= self->max_health)
	{
		self->health = self->max_health;
		assassin_reactivate(self);
	}
}

mframe_t assassin_frames_false_death[] = {
	{ ai_move, 0, assassin_heal }
};
MMOVE_T(assassin_move_false_death) = { FRAME_death5, FRAME_death5, assassin_frames_false_death, assassin_false_death };

void assassin_false_death(edict_t *self)
{
	M_SetAnimation(self, &assassin_move_false_death);
}

mframe_t assassin_frames_false_death_start[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_false_death_start) = { FRAME_death1, FRAME_death5, assassin_frames_false_death_start, assassin_false_death };

void assassin_false_death_start(edict_t *self)
{
	self->s.angles[2] = 0;
	self->gravityVector = { 0, 0, -1 };

	self->monsterinfo.aiflags |= AI_STAND_GROUND;
	M_SetAnimation(self, &assassin_move_false_death_start);
}
#endif
// ******************
// PAIN
// ******************

mframe_t assassin_frames_pain1[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_pain1) = { FRAME_pain1, FRAME_pain4, assassin_frames_pain1, assassin_run };

mframe_t assassin_frames_pain2[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_pain2) = { FRAME_pain5, FRAME_pain12, assassin_frames_pain2, assassin_run };

PAIN(assassin_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	if (self->deadflag)
		return;

	if (self->groundentity == nullptr)
		return;

#if 0 //KONIG - Saving this for Ionized, no good animations
	// if we're reactivating or false dying, ignore the pain.
	if (self->monsterinfo.active_move == &assassin_move_false_death_end ||
		self->monsterinfo.active_move == &assassin_move_false_death_start)
		return;

	if (self->monsterinfo.active_move == &assassin_move_false_death)
	{
		assassin_reactivate(self);
		return;
	}

	if ((self->health > 0) && (self->health < (self->max_health / 4)))
	{
		if (frandom() < 0.30f)
		{
			assassin_false_death_start(self);
			return;
		}
	}
#endif

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (mod.id == MOD_CHAINFIST || damage > 10) // don't react unless the damage was significant
	{
		// assassin should dodge jump periodically to help avoid damage.
		if (self->groundentity && (frandom() < 0.5f))
			assassin_dodge_jump(self);
		else if (M_ShouldReactToPain(self, mod)) // no pain anims in nightmare
			if (frandom() < 0.20f)
			{
				M_SetAnimation(self, &assassin_move_pain2);
			}
			else
			{
				M_SetAnimation(self, &assassin_move_pain1);
			}
	}
}

MONSTERINFO_SETSKIN(assassin_setskin) (edict_t *self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

// ******************
// STALKER ATTACK
// ******************

void assassin_shoot_attack(edict_t *self)
{
	vec3_t	offset, start, f, r, dir;
	vec3_t	end;
	float	dist;
	trace_t trace;

	if (!has_valid_enemy(self))
		return;

	if (self->groundentity && frandom() < 0.33f)
	{
		dir = self->enemy->s.origin - self->s.origin;
		dist = dir.length();

		if ((dist > 256) || (frandom() < 0.5f))
			assassin_do_pounce(self, self->enemy->s.origin);
		else
			assassin_jump_straightup(self);
	}

	AngleVectors(self->s.angles, f, r, nullptr);
	offset = { 24, 0, 6 };
	start = M_ProjectFlashSource(self, offset, f, r);

	dir = self->enemy->s.origin - start;
	if (frandom() < 0.3f)
		PredictAim(self, self->enemy, start, 1000, true, 0, &dir, &end);
	else
		end = self->enemy->s.origin;

	trace = gi.traceline(start, end, self, MASK_PROJECTILE);
	if (trace.ent == self->enemy || trace.ent == world)
	{
		dir.normalize();
		if (frandom() < 0.2f)
			monster_fire_redflare(self, start, dir, 100, 800, MZ2_SOLDIER_BLASTER_1);
		else
			monster_fire_yellowflare(self, start, dir, 50, 800, MZ2_SOLDIER_BLASTER_1);
	}
}

void assassin_shoot_attack2(edict_t *self)
{
	if (frandom() < 0.5)
		assassin_shoot_attack(self);
}

mframe_t assassin_frames_shoot[] = {
	{ ai_charge, 0, assassin_shoot_attack },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, assassin_shoot_attack2 },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(assassin_move_shoot) = { FRAME_attak1, FRAME_attak8, assassin_frames_shoot, assassin_run };

MONSTERINFO_ATTACK(assassin_attack_ranged) (edict_t *self) -> void
{
	if (!has_valid_enemy(self))
		return;

	// PMM - circle strafe stuff
	if (frandom() > 0.5f)
	{
		self->monsterinfo.attack_state = AS_STRAIGHT;
	}
	else
	{
		if (frandom() <= 0.5f) // switch directions
			self->monsterinfo.lefty = !self->monsterinfo.lefty;
		self->monsterinfo.attack_state = AS_SLIDING;
	}
	M_SetAnimation(self, &assassin_move_shoot);
}

// ******************
// POUNCE
// ******************

// ====================
// ====================
bool assassin_check_lz(edict_t *self, edict_t *target, const vec3_t &dest)
{
	if ((gi.pointcontents(dest) & MASK_WATER) || (target->waterlevel))
		return false;

	if (!target->groundentity)
		return false;

	vec3_t jumpLZ;

	// check under the player's four corners
	// if they're not solid, bail.
	jumpLZ[0] = self->enemy->mins[0];
	jumpLZ[1] = self->enemy->mins[1];
	jumpLZ[2] = self->enemy->mins[2] - 0.25f;
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	jumpLZ[0] = self->enemy->maxs[0];
	jumpLZ[1] = self->enemy->mins[1];
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	jumpLZ[0] = self->enemy->maxs[0];
	jumpLZ[1] = self->enemy->maxs[1];
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	jumpLZ[0] = self->enemy->mins[0];
	jumpLZ[1] = self->enemy->maxs[1];
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	return true;
}

// ====================
// ====================
bool assassin_do_pounce(edict_t *self, const vec3_t &dest)
{
	vec3_t	dist;
	float	length;
	vec3_t	jumpAngles;
	vec3_t	jumpLZ;
	float	velocity = 400.1f;

	if (!assassin_check_lz(self, self->enemy, dest))
		return false;

	dist = dest - self->s.origin;

	// make sure we're pointing in that direction 15deg margin of error.
	jumpAngles = vectoangles(dist);
	if (fabsf(jumpAngles[YAW] - self->s.angles[YAW]) > 45)
		return false; // not facing the player...

	if (isnan(jumpAngles[YAW]))
		return false; // Switch why

	self->ideal_yaw = jumpAngles[YAW];
	M_ChangeYaw(self);

	length = dist.length();
	if (length > 450)
		return false; // can't jump that far...

	jumpLZ = dest;
	vec3_t dir = dist.normalized();

	// find a valid angle/velocity combination
	while (velocity <= 800)
	{
		if (M_CalculatePitchToFire(self, jumpLZ, self->s.origin, dir, velocity, 3, false, true))
			break;

		velocity += 200;
	}

	// nothing found
	if (velocity > 800)
		return false;

	self->velocity = dir * velocity;
	return true;
}

// ******************
// DODGE
// ******************

//===================
// assassin_jump_straightup
//===================
void assassin_jump_straightup(edict_t *self)
{
	if (self->deadflag)
		return;

	if (self->groundentity) // make sure we're standing on SOMETHING...
	{
		self->velocity[0] += crandom() * 5;
		self->velocity[1] += crandom() * 5;
		self->velocity[2] += -400 * self->gravityVector[2];
		self->gravityVector[2] = 1;
		self->s.angles[2] = 180.0;
		self->groundentity = nullptr;
	}
}

mframe_t assassin_frames_jump_straightup[] = {
	{ ai_move, 1, assassin_jump_straightup },
	{ ai_move, 1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1, assassin_jump_wait_land },
	{ ai_move, -1, monster_footstep },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};

MMOVE_T(assassin_move_jump_straightup) = { FRAME_duck4, FRAME_duck18, assassin_frames_jump_straightup, assassin_run };

//===================
// assassin_dodge_jump - abstraction so pain function can trigger a dodge jump too without
//		faking the inputs to assassin_dodge
//===================
void assassin_dodge_jump(edict_t *self)
{
	M_SetAnimation(self, &assassin_move_jump_straightup);
}

#if 0
mframe_t assassin_frames_dodge_run[] = {
	{ ai_run, 13 },
	{ ai_run, 17 },
	{ ai_run, 21 },
	{ ai_run, 18, monster_done_dodge }
};
MMOVE_T(assassin_move_dodge_run) = { FRAME_run01, FRAME_run04, assassin_frames_dodge_run, nullptr };
#endif

MONSTERINFO_DODGE(assassin_dodge) (edict_t *self, edict_t *attacker, gtime_t eta, trace_t *tr, bool gravity) -> void
{
	if (!self->groundentity || self->health <= 0)
		return;

	if (!self->enemy)
	{
		self->enemy = attacker;
		FoundTarget(self);
		return;
	}

	// PMM - don't bother if it's going to hit anyway; fix for weird in-your-face etas (I was
	// seeing numbers like 13 and 14)
	if ((eta < FRAME_TIME_MS) || (eta > 5_sec))
		return;

	if (self->timestamp > level.time)
		return;

	self->timestamp = level.time + random_time(1_sec, 5_sec);
	// this will override the foundtarget call of assassin_run
	assassin_dodge_jump(self);
}

// ******************
// Jump onto / off of things
// ******************

//===================
//===================
void assassin_jump_down(edict_t *self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 100);
	self->velocity += (up * 300);
}

//===================
//===================
void assassin_jump_up(edict_t *self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 200);
	self->velocity += (up * 450);
}

//===================
//===================
void assassin_jump_wait_land(edict_t *self)
{
	if ((frandom() < 0.4f) && (level.time >= self->monsterinfo.attack_finished))
	{
		self->monsterinfo.attack_finished = level.time + 300_ms;
		assassin_shoot_attack(self);
	}

	if (self->groundentity == nullptr)
	{
		self->gravity = 1.3f;
		self->monsterinfo.nextframe = self->s.frame;

		if (monster_jump_finished(self))
		{
			self->gravity = 1;
			self->monsterinfo.nextframe = self->s.frame + 1;
		}
	}
	else
	{
		self->gravity = 1;
		self->monsterinfo.nextframe = self->s.frame + 1;
	}
}

mframe_t assassin_frames_jump_up[] = {
	{ ai_move, -8 },
	{ ai_move, -8 },
	{ ai_move, -8 },
	{ ai_move, 1, assassin_jump_straightup },
	{ ai_move, 1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1, assassin_jump_wait_land },
	{ ai_move, -1, monster_footstep },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_jump_up) = { FRAME_duck1, FRAME_duck18, assassin_frames_jump_up, assassin_run };

mframe_t assassin_frames_jump_down[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, assassin_jump_straightup },
	{ ai_move},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, assassin_jump_wait_land },
	{ ai_move, 0, monster_footstep },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_jump_down) = { FRAME_duck1, FRAME_duck18, assassin_frames_jump_down, assassin_run };

//============
// assassin_jump - this is only used for jumping onto or off of things. for dodge jumping,
//		use assassin_dodge_jump
//============
void assassin_jump(edict_t *self, blocked_jump_result_t result)
{
	if (!self->enemy)
		return;

	if (result == blocked_jump_result_t::JUMP_JUMP_UP)
		M_SetAnimation(self, &assassin_move_jump_up);
	else
		M_SetAnimation(self, &assassin_move_jump_down);
}

// ******************
// Blocked
// ******************
MONSTERINFO_BLOCKED(assassin_blocked) (edict_t *self, float dist) -> bool
{
	if (!has_valid_enemy(self))
		return false;
	
	return false;
}

// ******************
// Death
// ******************

void assassin_dead(edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

mframe_t assassin_frames_death[] = {
	{ ai_move, -5 },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_death) = { FRAME_death18, FRAME_death25, assassin_frames_death, assassin_dead };

DIE(assassin_die) (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, const vec3_t &point, const mod_t &mod) -> void
{
	// dude bit it, make him fall!
	self->movetype = MOVETYPE_TOSS;
	self->s.angles[2] = 0;
	self->gravityVector = { 0, 0, -1 };

	self->s.modelindex2 = 0;
	ThrowGib(self, "models/monsters/assassin/weapon.md2", damage, GIB_METALLIC, self->s.scale);

	// check for gib
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
		{ 2, "models/objects/gibs/sm_meat/tris.md2" },
		{ 3, "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
		{ "models/monsters/assassin/weapon.md2", GIB_METALLIC },
		{ "models/objects/gibs/chest/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD },
		});
		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	// regular death
	gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;
	M_SetAnimation(self, &assassin_move_death);
}

// ******************
// SPAWN
// ******************

/*QUAKED monster_assassin (1 .5 0) (-28 -28 -18) (28 28 18) Ambush Trigger_Spawn Sight  NoJumping
Spider Monster

*/

constexpr spawnflags_t SPAWNFLAG_STALKER_NOJUMPING = 16_spawnflag;

void SP_monster_assassin(edict_t *self)
{
	const spawn_temp_t &st = ED_GetSpawnTemp();

	if ( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	sound_pain.assign("assassin/pain.wav");
	sound_die.assign("assassin/death.wav");
	sound_sight.assign("assassin/sight.wav");
	sound_idle.assign("assassin/idle.wav");

	// PMM - precache bolt2
	gi.modelindex("models/objects/laser/tris.md2");

	self->s.modelindex = gi.modelindex("models/monsters/assassin/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/assassin/weapon.md2");

	gi.modelindex("models/monsters/assassin/gibs/bodya.md2");
	gi.modelindex("models/monsters/assassin/gibs/bodyb.md2");
	gi.modelindex("models/monsters/assassin/gibs/claw.md2");
	gi.modelindex("models/monsters/assassin/gibs/foot.md2");
	gi.modelindex("models/monsters/assassin/gibs/head.md2");
	gi.modelindex("models/monsters/assassin/gibs/leg.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 250 * st.health_multiplier;
	self->gib_health = -50;
	self->mass = 250;

	self->pain = assassin_pain;
	self->die = assassin_die;

	self->monsterinfo.stand = assassin_stand;
	self->monsterinfo.walk = assassin_walk;
	self->monsterinfo.run = assassin_run;
	self->monsterinfo.attack = assassin_attack_ranged;
	self->monsterinfo.sight = assassin_sight;
	self->monsterinfo.idle = assassin_idle;
	self->monsterinfo.dodge = assassin_dodge;
	self->monsterinfo.blocked = assassin_blocked;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.setskin = assassin_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &assassin_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.can_jump = !self->spawnflags.has(SPAWNFLAG_STALKER_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start(self);
}
