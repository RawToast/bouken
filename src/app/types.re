/* Basic types */
type stats = {
  health: int,
  speed: float,
  position: float,
  damage: int
};

type player = {
  name: string,
  stats: stats,
  gold: int,
  location: (int, int)
};

type ai = {
  moveRange: int,
  terrainCost: bool,
  mustSee: bool,
  memory: option((int, int)),
  attackRange: int,
  flying: bool,
  swim: bool,
  seedark: bool,
  small: bool
};

type enemy = {
  id: string,
  name: string,
  stats: stats,
  ai: ai
};

type enemyInfo = {
  enemy: enemy,
  location: (int, int)
};

type link = {
  id: int,
  level: string,
};

type tileEffect = 
  | Trap(int)
  | Snare(float)
  | Heal(int)
  | Gold(int)
  | NoEff;

type tile =
  | GROUND
  | ROUGH
  | STAIRS(link)
  | WATER
  | WALL
  | EXIT(int);

type occupier = 
  | Player(player)
  | Enemy(enemy)
  | Empty;

type place = {
  visible: bool,
  tile: tile,
  state: occupier,
  tileEffect: tileEffect
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
  | InvalidState
  | ImpossibleMove;

type actionResult = 
  | Ok(game)
  | EndGame(int, string)
  | Error(string);

let error = (err) => Js.Result.Error(err);

let success = (ok) => Js.Result.Ok(ok);

let isPlayer = place => switch place.state {
  | Player(_) => true
  | _ => false
  };

/* Modules */
module type World = {
  let updateLevel: (level, world) => world;
  let currentLevel: world => option(level);
  let selectLevel: (string, world) => option(level);
};

module type WorldBuilder = {
  let create: player => world;
};

module type WorldCreator = {
  let buildPlace: string => place;
  let buildArea: string => area;
  let buildLevel: (string, string) => level;
  let loadWorld: (string, string) => world;
  let loadWorldAsync: (string, string) => Js.Promise.t(world);
};

module type Places = {
  let findPlayer: (area) => option(player); 
  let findEnemy: (string, area) => option(enemy);
  let canMoveTo: (~overwrite:bool=?, int, int, area) => Js.Result.t(place, error);
  let removeOccupant: (int, int, area) => area;
  let movePlayer: (int, int, float, area) => Js.Result.t (playerArea, error);
  let setPlayerAt: (int, int, player, float, area) => Js.Result.t (area, error);
  let setEnemyAt: (int, int, enemy, float, area) => Js.Result.t (area, error);
  let getPlace: (int, int, area) => option(place);
  let findStairs: (int, area) => option(place);
  let locationOfStairs: (int, area) => option((int, int));
};

module type Movement = {
  let canNavigateTo: (~limit: int=?, area, (int, int), (int, int)) => bool;
  let suggestMove: (~limit: int=?, ~incTerrain: bool=?, area, (int, int), (int, int)) => (int, int);
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

module type EnemyLoop = {
  let findActiveEnemies: area => list(enemyInfo);
  let canAttack: (~range: int=?, area, enemyInfo) => bool;
  let attack: (enemyInfo, area) => option((area, player));
  let takeTurn: (enemyInfo, level, game) => option(game);
  let chase: (area, enemyInfo) => (int, int);
};

module type Game = {
  let create: string => game;
  let attack: (int, int, game) => actionResult;
  let movePlayer: (int, int, game) => actionResult;
  let useStairs: game => actionResult;
  let useExit: game => actionResult;
  let resultUpdateVision: actionResult => actionResult;

  let updateVision: game => game;
};

module type AsyncGame = {
  let create: string => Js.Promise.t(game);
  let attack: (int, int, game) => actionResult;
  let movePlayer: (int, int, game) => actionResult;
  let useStairs: game => actionResult;
  let useExit: game => actionResult;
};

module Operators = {
  let isOk = r => switch(r) {
    | Ok(_) => true
    | _ => false
    };
  
  /* flatMap on ActionResult */
  let (>>=) = (r, f) => switch(r) {
    | Ok(gam) => f(gam)
    | Error(err) => Error(err)
    | EndGame(score, name) => EndGame(score, name)
    };
  
  /* Default operator */
  let (|?) = (r, g) => switch(r) {
    | Ok(gam) => gam
    | _ => g
    };
};