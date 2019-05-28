The reference mesh here was modified by player_skeleton_convert.py to fix up the Nightfire model bones so that they were in the correct format for the existing Half Life animations. The fingers are still slightly screwy, so I'll probably need to account for that somehow.

Running studiomdl.exe with the mp_MI6_tux.qc file should output an MDL. Not all the animations are required by the QC - I need to go through and remove redundant ones soon.

This is the "work in progress" model for the conversion process. Once this model looks OK, we can convert the rest of the multiplayer models to the same format.