# Interaction And Player Data Audit

## Current Interaction Sources

- Static business/entrance data: `gamemodes/core/server/server.inc`, `storeData`, 35 entries.
- Entrance runtime/labels/prompt/teleport: `gamemodes/core/server/server_entrances.inc`.
- House runtime/labels/prompt/teleport: `gamemodes/core/houses/houses_impl.inc`, `houses.inc`, `houses_entext-handler.inc`.
- ATM runtime/labels/prompt/menu: `gamemodes/core/server/server_atm-impl.inc`, `server_atm-menu.inc`.
- Generic interaction helper: `gamemodes/core/interaction.inc`.
- Phone booth system: not present yet.
- Generic object interaction registry: not present yet.

## Static Entrance And Business Points

These are configured in `storeData` and generate both entrance and exit interaction points.

- GYM
- TEN GREEN BOTTLES
- CLUCKING BELL, 3 locations
- BINCO
- AMMU-NATION, 2 locations
- SEX SHOP, 2 locations
- GAS STATION, 2 locations
- BURGER SHOT, 2 locations
- BARBER, 2 locations
- POLICE DEPT.
- PIZZA
- STRIP CLUB
- DISCO
- 24/7, 2 locations
- TATTOO
- SUBURBAN
- VICTIM
- JIM'S STICKY DONUTS
- CITY HALL
- JEFFERSON MOTEL
- PROLAPS
- ZIP
- BANK
- CASINO
- XomoX's Mansion
- GROTTI
- DRUG HOUSE

## Dynamic Interaction Data

- Houses are loaded from `player_houses` through `scriptfiles/houses.sql`.
- ATMs are loaded from `atms` through `scriptfiles/atms.sql`.
- House/ATM exact positions are database data, so the runtime list depends on current DB rows.
- Current code now creates labels and prompts for every loaded house and ATM record.

## Existing Job And Object-Like Points

These systems already have their own labels, checkpoints, areas, or key handling and are candidates for later migration into the generic interaction module.

- Dealership: `gamemodes/core/server/dealership/dealership.inc`
- 24/7 and gas station shop areas: `gamemodes/core/server/server_shop-items.inc`
- Bomb shop: `gamemodes/core/systems/bomb_shop.inc`
- Hospital: `gamemodes/core/systems/hospital_system.inc`
- Slot machines: `gamemodes/core/systems/slot-machine_system.inc`
- Police mission point: `gamemodes/core/systems/jobs/police_mission.inc`
- Garbage collector factory/trash points: `gamemodes/core/systems/jobs/garbage_collector.inc`
- Farming market and orange farm: `gamemodes/core/systems/side-jobs/farming/`
- Dumpster diving: `gamemodes/core/systems/dumpster_diving.inc`

## Player Data And Database Structure

Table setup is coordinated by `gamemodes/core/tables.inc` and SQL files in `scriptfiles`.

- Account identity: `players.sql`, loaded through `account.inc`.
- Core stats: `player_stats.sql`, loaded through `account_stats.inc`.
- Money: `player_stats.money`, saved through `account.inc`.
- Job: `player_stats.job_id`, saved through `player_job.inc`.
- Class/wanted/score/kills/deaths: `player_stats`, loaded through `account_stats.inc`.
- XP/level: `player_stats.xp`, `player_stats.level`, handled by `player_xp.inc`.
- VIP: `vips.sql`, handled by `account_vip.inc`.
- Bank: `player_bank.sql`, handled by player bank modules.
- Inventory-like items: `player_items.sql`.
- Weapons: `weapons.sql`, `player_weapon-save.inc`, weapon dealer modules.
- Vehicle ownership: `vehicle_setup-table.inc`, `vehicle_impl.inc`.
- Houses and furniture: `houses.sql`, house modules.
- ATMs: `atms.sql`, ATM modules.

## Missing Data Areas For Later Work

- Licenses need new player data columns or a dedicated table.
- Driving tests need persistent license result data.
- Phones need phone number, contacts, call/message records, and payphone points.
- Job XP should be separated from global XP if each profession needs its own level.
