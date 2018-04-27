/* Types for a level */
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