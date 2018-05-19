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

type link = {
  id: int,
  level: string,
};

type tile =
  | GROUND
  | STAIRS(link)
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

type playerArea = {
  player: player,
  area: area,
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
module type Places = {
  let findPlayer: (area) => option(player); 
  let findEnemy: (string, area) => option(enemy);
  let canMoveTo: (int, int, area) => Js.Result.t(place, error);
  let removeOccupant: (int, int, area) => area;
  let movePlayer: (int, int, float, area) => Js.Result.t (playerArea, error);
};

module type Positions = {
  let divisor: float;
  let isActive: stats => bool;
  let increment: stats => stats;
  let incrementAll: area => area;
};

module type GameLoop = {
  let continue: game => game;
};

module type Game = {
  let create: string => game;
  let movePlayer: (int, int, game) => option(game)
};
