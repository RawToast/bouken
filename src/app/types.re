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
  id: string,
  name: string,
  stats: stats
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

type area = list(list(place));

type level = {
  name: string,
  map: area,
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

let isPlayer = place => switch place.state {
  | EMPTY => false
  | ENEMY(_) => false
  | PLAYER(_) => true
  };

/* Modules */

module type Positions = {
  let isActive: stats => bool;
  let incrementAll: area => area;
};

module type Game = {
  let create: string => game;
  let movePlayer: (int, int, game) => option(game)
};
