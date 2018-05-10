module StaticData = {
  open Level;
  open Types;
  let listHorizontal = (x, y, tx) => Rationale.RList.rangeInt(1, x, tx) |> List.map(i => (i, y));
  let listVertical = (x, y, ty) => Rationale.RList.rangeInt(1, y, ty) |> List.map(i => (x, i));

  let initPlayer = {name: "test", stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (6, 6)};
  let initEnemy = {name: "spooky thing", stats: { health: 3, speed: 1.0, position: 0. }};
  let initLevel =
    LevelBuilder.makeBlankWorld("test")
    |> Level.modifyTiles(listVertical(0, 0, 13), Tiles.wallTile)
    |> Level.modifyTiles(listHorizontal(0, 0, 14), Tiles.wallTile)
    |> Level.modifyTiles(listHorizontal(0, 14, 14), Tiles.wallTile)
    |> Level.modifyTiles(listVertical(2, 6, 12), Tiles.wallTile)
    |> Level.modifyTiles(listHorizontal(4, 12, 8), Tiles.wallTile)
    |> Level.modifyTiles(listVertical(14, 2, 14), Tiles.wallTile)
    |> Level.modifyTiles([(7, 6), (8, 6), (9, 6), (7, 7), (8, 7), (8, 8)], Tiles.wallTile)
    |> Level.modifyTile(9, 9, {tile: GROUND, state: ENEMY(initEnemy)})
    |> Level.modifyTile(6, 6, {tile: GROUND, state: PLAYER(initPlayer)});
  let initgame = {
    player: initPlayer,
    level: initLevel,
    turn: 0.
  }
};