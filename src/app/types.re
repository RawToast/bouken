/* Basic types */
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

type playerTile = {
  tile: tile,
  player: player
};

type enemyTile = {
  tile: tile,
  enemy: enemy
};

type level = {
  name: string,
  map: list(list(place)),
};

type error = 
  | INVALID_STATE
  | IMPOSSIBLE_MOVE;

type result('l, 'r) = 
  | Failure('l) 
  | Success('r);

module Result = {
  let mapf = (res, fn) =>
    switch res {
    | Failure(failure) => Failure(failure)
    | Success(success) => Success(fn(success))
    };

  let flatmapf = (res, fn:'b => result('a, 'c)) =>
    mapf(res, fn)
      |> rs => switch rs {
      | Failure(f) => Failure(f)
      | Success(s) => s
    };
};