
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "Cow.h"
#include "../Entities/Player.h"







cCow::cCow(void) :
	super("Cow", mtCow, "mob.cow.hurt", "mob.cow.hurt", 0.9, 1.3)
{
}





void cCow::GetDrops(cItems & a_Drops, cEntity * a_Killer)
{
	int LootingLevel = 0;
	if (a_Killer != NULL)
	{
		LootingLevel = a_Killer->GetEquippedWeapon().m_Enchantments.GetLevel(cEnchantments::enchLooting);
	}
	AddRandomDropItem(a_Drops, 0, 2 + LootingLevel, E_ITEM_LEATHER);
	AddRandomDropItem(a_Drops, 1, 3 + LootingLevel, IsOnFire() ? E_ITEM_STEAK : E_ITEM_RAW_BEEF);
}

