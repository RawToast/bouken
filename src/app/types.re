/* Basic types */

type stats = {
  health: int,
  speed: float,
  position: float
};
type player = {
  name: string,
  stats: stats,
  gold: int,
  location: (int, int)
};

type enemy = {
  name: string,
  stats: stats,
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

type game = {
  player: player,
  level: level,
  turn: float
};

type error = 
  | INVALID_STATE
  | IMPOSSIBLE_MOVE;

let error = (err) => Js.Result.Error(err);

let success = (ok) => Js.Result.Ok(ok);
