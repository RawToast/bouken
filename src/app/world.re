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

module FetchCsvBuilder = {
  open Level;
  open Rationale;
  
  let create = (player: player) => {
    let (x, y) = player.location;

    let d1 = Utils.requireAssetURI("../../public/world/Dungeon 1.csv");
    let d2 = Utils.requireAssetURI("../../public/world/Dungeon 2.csv");
    let d3 = Utils.requireAssetURI("../../public/world/Dungeon 3.csv");
    let d4 = Utils.requireAssetURI("../../public/world/Dungeon 4.csv");
  
    let d5 = Utils.requireAssetURI("../../public/world/Dungeon 5.csv");
    let c1 = Utils.requireAssetURI("../../public/world/Cave.csv");
    let s1 = Utils.requireAssetURI("../../public/world/Swamp.csv");
    let l1 = Utils.requireAssetURI("../../public/world/Labyrinth.csv");

    let lvls = [("Dungeon 1", d1), ("Dungeon 2", d2), ("Dungeon 3", d3), 
                ("Dungeon 4", d4), ("Dungeon 5", d5), ("Cave", d5), 
                ("Swamp", s1), ("Labyrinth", l1)];
    
    Worldcreator.CsvWorldBuilder
      .loadWorldAsync("Dungeon 1", lvls)
      |> Js.Promise.then_(world => 
        world 
          |> World.currentLevel
          |> Option.fmap(Level.modifyTile(x, y, {tile: GROUND, state: Player(player), tileEffect: NoEff, visible: false }))
          |> Option.fmap(World.updateLevel(_, world))
          |> Option.default(world) 
          |> Js.Promise.resolve);
  };
};

module Builder: WorldBuilder = {
  open Level;
  open Rationale;

  let listHorizontal = (x, y, tx) => RList.rangeInt(1, x, tx) |> List.map(i => (i, y));
  let listVertical = (x, y, ty) => RList.rangeInt(1, y, ty) |> List.map(i => (x, i));
  let initEnemy = Enemy.Enemies.makeEnemy();
  let newEnemy = id => { ... initEnemy,id: id, name: id };
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
        |> Level.modifyTile(9, 9, {tile: GROUND, state: Enemy(initEnemy), tileEffect: NoEff, visible: false })
        |> Level.modifyTile(6, 6, {tile: GROUND, state: Player(player), tileEffect: NoEff, visible: false })
        |> Level.modifyTile(13, 14, { tile: STAIRS({ id: 1, level: "Dungeon 2" }), state: Empty, tileEffect: NoEff, visible: false })
        |> Level.modifyTile(9, 5, { tile: STAIRS({ id: 0, level: "Swamp" }), state: Empty, tileEffect: NoEff, visible: false });


    let swamp = LevelBuilder.makeWaterLevel("Swamp")
      |> Level.modifyTile(0, 9, { tile: STAIRS({ id: 0, level: "Dungeon 1" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 1, level: "Cave" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile)
      |> Level.modifyTile(4, 10, {tile: GROUND, state: Enemy(newEnemy("z1")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(9, 8, {tile: GROUND, state: Enemy(newEnemy("z2")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(11, 2, {tile: GROUND, state: Enemy(newEnemy("z3")), tileEffect: NoEff, visible: false });

    let cave = LevelBuilder.makeLevel("Cave", 20, 6, WALL)
      |> Level.modifyTiles(listHorizontal(1, 1, 19), Tiles.groundTile)
      |> Level.modifyTiles(listHorizontal(1, 2, 17), Tiles.groundTile)
      |> Level.modifyTiles([(19, 6), (18, 5), (17, 6), (19, 2), (16, 4), (18, 4), (19, 3)], Tiles.groundTile)
      |> Level.modifyTiles([(8, 5), (9, 6), (4, 4), (5, 4), (13, 4), (14, 4)], Tiles.groundTile)
      |> Level.modifyTile(19, 5, { tile: STAIRS({ id: 2, level: "Dungeon 4" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(0, 3, { tile: STAIRS({ id: 1, level: "Swamp" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile)
      |> Level.modifyTile(8, 2, {tile: GROUND, state: Enemy(newEnemy("z1")), tileEffect: NoEff, visible: false });

    let level2 = LevelBuilder.makeBlankLevel("Dungeon 2")
      |> Level.modifyTile(0, 9, { tile: STAIRS({ id: 1, level: "Dungeon 1" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 0, level: "Dungeon 3" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(3, 3, {tile: GROUND, state: Enemy(newEnemy("z1")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(11, 9, {tile: GROUND, state: Enemy(newEnemy("z2")), tileEffect: NoEff, visible: false })
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.groundTile);

    let level3 = LevelBuilder.makeLevel("Dungeon 3", 18, 20, GROUND)
      |> Level.modifyTile(5, 18, { tile: STAIRS({ id: 1, level: "Dungeon 4" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(13, 1, { tile: STAIRS({ id: 0, level: "Dungeon 2" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(5, 8, { tile: EXIT(500), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (4, 8), (6, 8)], Tiles.wallTile)
      |> Level.modifyTile(3, 4, {tile: GROUND, state: Enemy(newEnemy("z1")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(4, 7, {tile: GROUND, state: Enemy(newEnemy("z2")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(8, 10, {tile: GROUND, state: Enemy(newEnemy("z3")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(15, 9, {tile: GROUND, state: Enemy(newEnemy("z4")), tileEffect: NoEff, visible: false })

    let level4 = LevelBuilder.makeBlankLevel("Dungeon 4")
      |> Level.modifyTile(2, 3, { tile: STAIRS({ id: 1, level: "Dungeon 3" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(13, 12, { tile: STAIRS({ id: 0, level: "Dungeon 5" }), state: Enemy(newEnemy("z4")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(7, 14, { tile: STAIRS({ id: 2, level: "Cave" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(13, 11, {tile: GROUND, state: Enemy(newEnemy("z1")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(0, 1, {tile: GROUND, state: Enemy(newEnemy("z2")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(12, 0, {tile: GROUND, state: Enemy(newEnemy("z3")), tileEffect: NoEff, visible: false });

    let level5 = LevelBuilder.makeLevel("Dungeon 5", 18, 20, GROUND)
      |> Level.modifyTile(5, 18, { tile: STAIRS({ id: 0, level: "Dungeon 4" }), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTile(15, 1, { tile: EXIT(1000), state: Empty, tileEffect: NoEff, visible: false })
      |> Level.modifyTiles([(4, 6), (5, 6), (6, 6), (7, 6), (5, 7), (6, 7), (5, 8), (6, 8)], Tiles.waterTile)
      |> Level.modifyTile(15, 2, {tile: GROUND, state: Enemy(newEnemy("z1")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(14, 0, {tile: GROUND, state: Enemy(newEnemy("z2")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(10, 10, {tile: GROUND, state: Enemy(newEnemy("z3")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(6, 15, {tile: GROUND, state: Enemy(newEnemy("z4")), tileEffect: NoEff, visible: false })
      |> Level.modifyTile(18, 11, {tile: GROUND, state: Enemy(newEnemy("z5")), tileEffect: NoEff, visible: false });

    { levels: [ initLevel(player), swamp, cave, level2, level3, level4, level5], current: "Dungeon 1"  }
  };
};