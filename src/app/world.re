open Types;

module World: World = {
  let updateLevel = (level, world) => { 
    let newLevels = world.levels 
      |> List.map(l => if (level.name == l.name) { level } else { l });
    
    { ... world, levels: newLevels };
  };

  let selectLevel = (name, world) => world.levels |> Rationale.RList.find(l => l.name == name);

  let currentLevel = (world) => selectLevel(world.current, world);
};


module Builder: WorldBuilder = {
  open Level;
  open Rationale;

  let listHorizontal = (x, y, tx) => RList.rangeInt(1, x, tx) |> List.map(i => (i, y));
  let listVertical = (x, y, ty) => RList.rangeInt(1, y, ty) |> List.map(i => (x, i));
  let initEnemy = {id: "testenemy", name: "spooky thing", stats: { health: 6, speed: 0.5, position: 0. }};
  let newEnemy = id => {id: id, name: id, stats: { health: 3, speed: 0.8, position: 0. }};
  let create = player => {
    let initLevel = player =>
        LevelBuilder.makeBlankLevel("Dungeon 1")
        |> Level.modifyTiles(listVertical(0, 0, 13), Tiles.wallTile)
        |> Level.modifyTiles(listHorizontal(0, 0, 14), Tiles.wallTile)
        |> Level.modifyTiles(listHorizontal(0, 14, 14), Tiles.wallTile)
        |> Level.modifyTiles(listVertical(2, 6, 12), Tiles.wallTile)
        |> Level.modifyTiles(listHorizontal(4, 12, 8), Tiles.wallTile)
        |> Level.modifyTiles(listVertical(14, 2, 14), Tiles.wallTile)
        |> Level.modifyTiles([(7, 6), (8, 6), (9, 6), (7, 7), (8, 7), (8, 8)], Tiles.wallTile)
        |> Level.modifyTiles([(7, 5), (8, 5), (9, 5), (7, 4), (8, 4), (8, 3)], Tiles.waterTile)
        |> Level.modifyTile(9, 9, {tile: GROUND, state: Enemy(initEnemy)})
        |> Level.modifyTile(6, 6, {tile: GROUND, state: Player(player)})
        |> Level.modifyTile(13, 14, { tile: STAIRS({ id: 1, level: "Dungeon 2" }), state: Empty})
        |> Level.modifyTile(9, 5, { tile: STAIRS({ id: 0, level: "Swamp" }), state: Empty});


    let swamp = LevelBuilder.makeWaterLevel("Swamp")
      |> Level.modifyTile(0, 9, { tile: STAIRS({ id: 0, level: "Dungeon 1" }), state: Empty})
      |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 1, level: "Cave" }), state: Empty})
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile)
      |> Level.modifyTile(4, 10, {tile: GROUND, state: Enemy(newEnemy("z1"))})
      |> Level.modifyTile(9, 8, {tile: GROUND, state: Enemy(newEnemy("z2"))})
      |> Level.modifyTile(11, 2, {tile: GROUND, state: Enemy(newEnemy("z3"))});

    let cave = LevelBuilder.makeLevel("Cave", 20, 6, WALL)
      |> Level.modifyTiles(listHorizontal(1, 1, 19), Tiles.groundTile)
      |> Level.modifyTiles(listHorizontal(1, 2, 17), Tiles.groundTile)
      |> Level.modifyTiles([(19, 6), (18, 5), (17, 6), (19, 2), (16, 4), (18, 4), (19, 3)], Tiles.groundTile)
      |> Level.modifyTiles([(8, 5), (9, 6), (4, 4), (5, 4), (13, 4), (14, 4)], Tiles.groundTile)
      |> Level.modifyTile(19, 5, { tile: STAIRS({ id: 2, level: "Dungeon 4" }), state: Empty})
      |> Level.modifyTile(0, 3, { tile: STAIRS({ id: 1, level: "Swamp" }), state: Empty})
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile)
      |> Level.modifyTile(8, 2, {tile: GROUND, state: Enemy(newEnemy("z1"))});

    let level2 = LevelBuilder.makeBlankLevel("Dungeon 2")
      |> Level.modifyTile(0, 9, { tile: STAIRS({ id: 1, level: "Dungeon 1" }), state: Empty})
      |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 0, level: "Dungeon 3" }), state: Empty})
      |> Level.modifyTile(2, 2, {tile: GROUND, state: Enemy(newEnemy("z1"))})
      |> Level.modifyTile(12, 12, {tile: GROUND, state: Enemy(newEnemy("z2"))})
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile);

    let level3 = LevelBuilder.makeLevel("Dungeon 3", 18, 20, GROUND)
      |> Level.modifyTile(5, 18, { tile: STAIRS({ id: 1, level: "Dungeon 4" }), state: Empty})
      |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 0, level: "Dungeon 2" }), state: Empty})
      |> Level.modifyTile(5, 8, { tile: EXIT(500), state: Empty})
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (4, 8), (6, 8)], Tiles.wallTile)
      |> Level.modifyTile(3, 4, {tile: GROUND, state: Enemy(newEnemy("z1"))})
      |> Level.modifyTile(4, 7, {tile: GROUND, state: Enemy(newEnemy("z2"))})
      |> Level.modifyTile(16, 17, {tile: GROUND, state: Enemy(newEnemy("z3"))})
      |> Level.modifyTile(15, 9, {tile: GROUND, state: Enemy(newEnemy("z4"))})

    let level4 = LevelBuilder.makeBlankLevel("Dungeon 4")
      |> Level.modifyTile(2, 3, { tile: STAIRS({ id: 1, level: "Dungeon 3" }), state: Empty})
      |> Level.modifyTile(13, 12, { tile: STAIRS({ id: 0, level: "Dungeon 5" }), state: Empty})
      |> Level.modifyTile(7, 14, { tile: STAIRS({ id: 2, level: "Cave" }), state: Empty})
      |> Level.modifyTile(13, 11, {tile: GROUND, state: Enemy(newEnemy("z1"))})
      |> Level.modifyTile(0, 1, {tile: GROUND, state: Enemy(newEnemy("z2"))})
      |> Level.modifyTile(12, 0, {tile: GROUND, state: Enemy(newEnemy("z3"))});

    let level5 = LevelBuilder.makeLevel("Dungeon 5", 18, 20, GROUND)
      |> Level.modifyTile(5, 18, { tile: STAIRS({ id: 0, level: "Dungeon 4" }), state: Empty})
      |> Level.modifyTile(15, 1, { tile: EXIT(1000), state: Empty})
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.waterTile)
      |> Level.modifyTile(15, 2, {tile: GROUND, state: Enemy(newEnemy("z1"))})
      |> Level.modifyTile(14, 0, {tile: GROUND, state: Enemy(newEnemy("z2"))})
      |> Level.modifyTile(10, 10, {tile: GROUND, state: Enemy(newEnemy("z3"))})
      |> Level.modifyTile(6, 15, {tile: GROUND, state: Enemy(newEnemy("z4"))})
      |> Level.modifyTile(18, 11, {tile: GROUND, state: Enemy(newEnemy("z5"))});

    { levels: [ initLevel(player), swamp, cave, level2, level3, level4, level5], current: "Dungeon 1"  }
  };
};