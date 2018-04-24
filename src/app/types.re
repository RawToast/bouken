/* Types for a level */
type player = {
  name: string,
  health: int,
  gold: int,
};

type enemy = {
  name: string,
  health: int,
};

type item = {name: string};

type tile =
  | GROUND
  | WATER
  | WALL;

type place = {
  tile: tile,
  items: list(item),
  enemies: list(enemy),
};

type level = {
  name: string,
  map: list(list(place)),
};