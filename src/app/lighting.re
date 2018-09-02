/*
Just some thoughts on lighting before commiting to code!


Lightning is on a scale from 1 - 10 (maybe a float for greater precision)
Each map has a light level, defaulting to 'full lighting' (8)

A light source is typically reduced by '1' per tile travelled:

* Note that this is until it reaches the level's light level
* Could have 'fog' reducing light travel.

What level is deemed 'adequately lit'? 8? 
* Higher levels can EVENTUALLY be used for enemy noticing the player
Likewise, under what level can the player not see? 3?
* Some enemies may see in the dark, with differing proficiencies

Tiles & Light Sources
* Wall torches?
* Fire - hurts to stand in, maybe gets put out (like a trap) by standing in it
* Lava - hurts to stand in, less light than fire, perpetual
* Enemies that emit light?

Steps

* Level brightness factor
* Only affect player vision
* Additional light sources
* 'Railings' or 'Glass walls', walls that allow light can travel through 
* Affect enemy sight/can see
* Attack penalties for attacking something that is dark
* Blinding levels of light???
*/