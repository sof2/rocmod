// Copyright (C) 2001-2002 Raven Software.
//
// gt_local.h -- local definitions for gametype module

#include "../../game/q_shared.h"
#include "../gt_public.h"
#include "../gt_syscalls.h"

typedef struct gametypeLocals_s
{
	int		time;

	int		carrier;
	int		carrierTime;
	vec3_t	carrierOrigin;

	int		caseTakenSound;
	int		caseCarrySound;
	int		caseReturnSound;

} gametypeLocals_t;

extern	gametypeLocals_t	gametype;

