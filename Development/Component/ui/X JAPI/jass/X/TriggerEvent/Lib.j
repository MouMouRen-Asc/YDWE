#pragma once

native X_TriggerRegisterPlayerUnitEvent 		takes trigger whichTrigger, player whichPlayer, integer eventID, boolexpr filter returns event
native X_TriggerRegisterPlayerUnitEventSimple 	takes trigger whichTrigger, player whichPlayer, integer eventID returns event
native X_GetTriggerUnit                         takes nothing returns unit
native X_GetTriggerPlayer 						takes nothing returns player
