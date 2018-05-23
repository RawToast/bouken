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
  | WALL
  | EXIT(int);

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

type world = {
  levels: list(level),
  current: string
};

type game = {
  player: player,
  world: world,
  turn: float
};

type error = 
  | INVALID_STATE
  | IMPOSSIBLE_MOVE;

type exitResult = 
  | END_GAME(int)
  | CONTINUE_GAME(game);

let error = (err) => Js.Result.Error(err);

let success = (ok) => Js.Result.Ok(ok);

let isPlayer = place => switch place.state {
  | EMPTY => false
  | ENEMY(_) => false
  | PLAYER(_) => true
  };

/* Modules */
module type World = {
  let create: player => world;
  let updateLevel: (level, world) => world;
  let currentLevel: world => option(level);
  let selectLevel: (string, world) => option(level);
};

module type Places = {
  let findPlayer: (area) => option(player); 
  let findEnemy: (string, area) => option(enemy);
  let canMoveTo: (int, int, area) => Js.Result.t(place, error);
  let removeOccupant: (int, int, area) => area;
  let movePlayer: (int, int, float, area) => Js.Result.t (playerArea, error);
  let setPlayerAt: (int, int, player, float, area) => Js.Result.t (area, error);
  let getPlace: (int, int, area) => option(place);
  let findStairs: (int, area) => option(place);
  let locationOfStairs: (int, area) => option((int, int));
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
  let movePlayer: (int, int, game) => option(game);
  let useStairs: game => option(game);
  let useExit: game => exitResult;
};
