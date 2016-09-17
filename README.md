The Call of Duty extension *libcod* is adding new server-side functions to:

 - Call Of Duty 2 1.0
 - Call Of Duty 2 1.2
 - Call Of Duty 2 1.3

Requirements:
```
gcc-multilib
g++-multilib
libmysqlclient-dev:i386 for MySQL (optional)
```

Precompiled shared libraries (outdated): http://killtube.org/downloads/libcod/

Starting the server:

```
export LD_PRELOAD="$HOME/cod2_1_0/libcod2_1_0.so"
./cod2_lnxded +set fs_game ... +set dedicated 2 ...
```
	
Working with the source / Compiling:
```
./doit.sh tar
./doit.sh cod2_1_0
./doit.sh cod2_1_2
./doit.sh cod2_1_3
```

Mods depending on *libcod*:
- http://killtube.org/showthread.php?1533-DOWNLOAD-CoD2-Surf-Mod
- http://killtube.org/showthread.php?1527-DOWNLOAD-CoD2-Basetdm-Mod
- http://killtube.org/showthread.php?1593-DOWNLOAD-CoD2-Portal-Mod
- Jump-Mod by IzNoGod
	
Little overview of added functions:
- MySQL
- setVelocity, getVelocity, addVelocity (needed for the mods: portal, surf)
- keyPressed-functions for left, right, forward, backward, leanleft, leanright, jump etc., (needed for: surf, doublejump made by IzNoGod)
- setAlive-function (needed for: zombots, so xmodels are damagable without damage-trigger... zombots in stockmaps)
- disableGlobalPlayerCollision() also disableGlobalPlayerEject()
- native bot functions (bots can move, shoot, jump, melee, ads, set stance)

Engine fixes:
- Faster download for CoD2 1.0
- Directory traversal fix
- Weapon struct segfault fix
- Rate Limiter
	
Community / Help: http://killtube.org/forum.php

CoD1 users:
Libcod support for CoD1 was very basic and without almost all functions and it's been dropped.
Use php's CoDExtended: https://github.com/riicchhaarrd/CoDExtended

CoD4 users: use CoD4x: https://github.com/callofduty4x/CoD4x_Server