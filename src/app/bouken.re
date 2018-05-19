module CreateGame: ((Types.GameLoop) => (Types.Game)) = (TL: Types.GameLoop) => {
    
  open Rationale;
  open Level;
  open Types;

  let listHorizontal = (x, y, tx) => RList.rangeInt(1, x, tx) |> List.map(i => (i, y));
  let listVertical = (x, y, ty) => RList.rangeInt(1, y, ty) |> List.map(i => (x, i));

  let initPlayer = name => {name: name, stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (6, 6)};
  let initEnemy = {id: "testenemy", name: "spooky thing", stats: { health: 3, speed: 1.0, position: 0. }};
  let initLevel = player =>
      LevelBuilder.makeBlankLevel("Floor 1")
      |> Level.modifyTiles(listVertical(0, 0, 13), Tiles.wallTile)
      |> Level.modifyTiles(listHorizontal(0, 0, 14), Tiles.wallTile)
      |> Level.modifyTiles(listHorizontal(0, 14, 14), Tiles.wallTile)
      |> Level.modifyTiles(listVertical(2, 6, 12), Tiles.wallTile)
      |> Level.modifyTiles(listHorizontal(4, 12, 8), Tiles.wallTile)
      |> Level.modifyTiles(listVertical(14, 2, 14), Tiles.wallTile)
      |> Level.modifyTiles([(7, 6), (8, 6), (9, 6), (7, 7), (8, 7), (8, 8)], Tiles.wallTile)
      |> Level.modifyTiles([(7, 5), (8, 5), (9, 5), (7, 4), (8, 4), (8, 3)], Tiles.waterTile)
      |> Level.modifyTile(9, 9, {tile: GROUND, state: ENEMY(initEnemy)})
      |> Level.modifyTile(6, 6, {tile: GROUND, state: PLAYER(player)})
      |> Level.modifyTile(13, 14, { tile: STAIRS({ id: 0, level: "Floor 2" }), state: EMPTY});

  let level2 = LevelBuilder.makeWaterLevel("Floor 2")
    |> Level.modifyTile(0, 9, { tile: STAIRS({ id: 0, level: "Floor 1" }), state: EMPTY})
    |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 1, level: "Floor 3" }), state: EMPTY})
    |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile);

  let level3 = LevelBuilder.makeBlankLevel("Floor 3")
  |> Level.modifyTile(3, 3, { tile: STAIRS({ id: 1, level: "Floor 2" }), state: EMPTY});


  let initgame = pname => initPlayer(pname) |> p => {
      player: p,
      world: { levels: [ initLevel(p), level2, level3 ], current: "Floor 1"  },
      turn: 0.
      };

  let create = name => initgame(name);

  let movePlayer = (x, y, game) => {

      let level = World.World.currentLevel(game.world);

      level |> 
      Option.bind(_, l =>
          l.map |> Area.movePlayer(x, y, 1.) |>
              Option.ofResult |> o => switch o {
                  | Some(area) => Some(area)
                  | None => None
                  }
              |> Option.fmap((pa:playerArea) => {
                  let xx = { ... l, map: pa.area};

                  let world = World.World.updateLevel(xx, game.world);

                  {... game, player: pa.player, world: world };
              }))
              |> Option.fmap(TL.continue);
  };
};
