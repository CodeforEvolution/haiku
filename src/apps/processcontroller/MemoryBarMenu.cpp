/*
 * Copyright 2000, Georges-Edouard Berenger. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include "MemoryBarMenu.h"
#include "MemoryBarMenuItem.h"
#include "KernelMemoryBarMenuItem.h"
#include "ProcessController.h"

#include <Bitmap.h>
#include <Roster.h>
#include <StringForSize.h>
#include <Window.h>

#include <stdlib.h>


#define EXTRA 10

float gMemoryTextWidth;


MemoryBarMenu::MemoryBarMenu(const char* name, info_pack* infos, system_info& systemInfo)
	: BMenu(name),
	fFirstShow(true)
{
	fTeamCount = systemInfo.used_teams + EXTRA;
	SetFlags(Flags() | B_PULSE_NEEDED);

	fTeamList = (team_id*)malloc(sizeof (team_id) * fTeamCount);

	unsigned int k;
	for (k = 0; k < systemInfo.used_teams; k++) {
		fTeamList[k] = infos[k].team_info.team;
	}

	while (k < fTeamCount) {
		fTeamList[k++] = -1;
	}

	char buffer[64];
	string_for_size(99999999.9, buffer, sizeof(buffer));
	gMemoryTextWidth = 2 * StringWidth(buffer) + 32;

	fRecycleCount = EXTRA;
	fRecycleList = (MRecycleItem*)malloc(sizeof(MRecycleItem) * fRecycleCount);
	SetFont(be_plain_font);
	AddItem(new KernelMemoryBarMenuItem(systemInfo));
}


MemoryBarMenu::~MemoryBarMenu()
{
	free(fTeamList);
	free(fRecycleList);
}


void
MemoryBarMenu::Draw(BRect updateRect)
{
	if (fFirstShow) {
		Pulse();
		fFirstShow = false;
	}

	BMenu::Draw(updateRect);
}


void
MemoryBarMenu::Pulse()
{
	system_info	sinfo;
	get_system_info(&sinfo);
	int64 committedMemory = (int64)sinfo.used_pages * B_PAGE_SIZE / 1024;
	int64 cachedMemory = (int64)sinfo.cached_pages * B_PAGE_SIZE / 1024;
	Window()->BeginViewTransaction();

	// create the list of items to remove, for their team is gone. Update the old teams.
	int lastRecycle = 0;
	int firstRecycle = 0;
	int	k;
	MemoryBarMenuItem* item;
	int	total = 0;
	for (k = 1; (item = (MemoryBarMenuItem*)ItemAt(k)) != NULL; k++) {
		int64 m = item->UpdateSituation(committedMemory);
		if (m < 0) {
			if (lastRecycle == fRecycleCount) {
				fRecycleCount += EXTRA;
				fRecycleList = (MRecycleItem*)realloc(fRecycleList,
					sizeof(MRecycleItem) * fRecycleCount);
			}
			fRecycleList[lastRecycle].index = k;
			fRecycleList[lastRecycle++].item = item;
		} else {
			if (lastRecycle > 0) {
				RemoveItems(fRecycleList[0].index, lastRecycle, true);
				k -= lastRecycle;
				lastRecycle = 0;
			}
			total += m;
		}
	}

	// Look new teams that have appeared. Create an item for them, or recycle from the list.
	int32 cookie = 0;
	info_pack infos;
	item = NULL;
	while (get_next_team_info(&cookie, &infos.team_info) == B_OK) {
		unsigned int j = 0;
		while (j < fTeamCount && infos.team_info.team != fTeamList[j]) {
			j++;
		}

		if (j == fTeamCount) {
			// new team
			team_info info;
			j = 0;
			while (j < fTeamCount && fTeamList[j] != -1) {
				if (get_team_info(fTeamList[j], &info) != B_OK)
					fTeamList[j] = -1;
				else
					j++;
			}

			if (j == fTeamCount) {
				fTeamCount += 10;
				fTeamList = (team_id*)realloc(fTeamList, sizeof(team_id) * fTeamCount);
			}

			fTeamList[j] = infos.team_info.team;
			if (!get_team_name_and_icon(infos, true)) {
				// the team is already gone!
				fTeamList[j] = -1;
			} else {
				if (!item && firstRecycle < lastRecycle)
					item = fRecycleList[firstRecycle++].item;

				if (item)
					item->Reset(infos.team_name, infos.team_info.team, infos.team_icon, true);
				else {
					AddItem(item = new MemoryBarMenuItem(infos.team_name,
						infos.team_info.team, infos.team_icon, true, NULL));
				}

				int m = item->UpdateSituation(committedMemory);
				if (m >= 0) {
					total += m;
					item = NULL;
				} else
					fTeamList[j] = -1;
			}
		}
	}

	if (item) {
		RemoveItem(item);
		delete item;
	}

	// Delete the items that haven't been recycled.
	if (firstRecycle < lastRecycle) {
		RemoveItems(IndexOf(fRecycleList[firstRecycle].item),
			lastRecycle - firstRecycle, true);
	}

	fLastTotalTime = system_time();
	KernelMemoryBarMenuItem	*kernelItem;
	if ((kernelItem = (KernelMemoryBarMenuItem*)ItemAt(0)) != NULL)
		kernelItem->UpdateSituation(committedMemory, cachedMemory);

	Window()->EndViewTransaction();
	Window()->Flush();
}
