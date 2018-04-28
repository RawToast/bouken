/* Basic types */
open Js.Result;

type player = {
  name: string,
  health: int,
  gold: int,
  location: (int, int)
};

type enemy = {
  name: string,
  health: int,
};

type tile =
  | GROUND
  | WATER
  | WALL;

type occupier = 
  | PLAYER(player)
  | ENEMY(enemy)
  | EMPTY;

type place = {
  tile: tile,
  state: occupier
};

type level = {
  name: string,
  map: list(list(place)),
};

type error = 
  | INVALID_STATE
  | IMPOSSIBLE_MOVE;

let error = (err) => Js.Result.Error(err);

let success = (ok) => Js.Result.Ok(ok);
