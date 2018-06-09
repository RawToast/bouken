open Types;

open Jest;
open Expect;

module EnemyLoop = Modules.EnemyLoop;
module Game = Modules.Game;

describe("EnemyLoop", () => {
  let game = Game.create("dave");

  let quickEnemy (id, position) = { id: id, name: "default",
    stats: { health: 1, speed: 1.0, position: position } };

  let activeEnemy = { id: "default", name: "default",
    stats: { health: 1, speed: 1.0, position: (1.) } };

  let blankLevel = Level.LevelBuilder.makeBlankLevel("Dungeon 1");

  let gameWithEnemyDelta = (x, y, game) => {
    let (px, py) = game.player.location;
    let newLevel = blankLevel
      |> Level.Level.modifyTile(
        px + x, py + y, 
        { tile: GROUND, state: Enemy(activeEnemy) })
      |> Level.Level.modifyTile(
        px, py, 
        { tile: GROUND, state: Player(game.player) });
    let newWorld = World.World.updateLevel(newLevel, game.world);
    ({ ... game, world: newWorld}, newLevel);
  };

  let gameWithAttackablePlayer = gameWithEnemyDelta(0, 1);

  describe("findActiveEnemies", () => {
    let level = blankLevel
        |> Level.Level.modifyTile(0, 5, { tile: GROUND, state: Enemy(activeEnemy)})
        |> Level.Level.modifyTile(10, 10, { tile: GROUND, state: Enemy({...activeEnemy, id: "alt"})});

    let activeEnemies = EnemyLoop.findActiveEnemies(level.map);

    test("Finds any active enemies", (_) => {
        expect(List.length(activeEnemies)) |> toEqual(2);
    });

    test("Includes the first enemies location", (_) => {
      let activeEnemies = EnemyLoop.findActiveEnemies(level.map);

      expect(activeEnemies 
        |> Rationale.RList.any(ei => {
          let (x, y) = ei.location;
          (x == 10) && (y == 10)}))
        |> toBe(true);
    });

    test("Includes the second enemies location", (_) => {
      let activeEnemies = EnemyLoop.findActiveEnemies(level.map);

      expect(activeEnemies 
        |> Rationale.RList.any(ei => {
          let (x, y) = ei.location;
          (x == 0) && (y == 5)}))
        |> toBe(true);
    });
  });

  describe("takeTurn", () => {
    open Rationale.Option;
    let active = quickEnemy("1", 1.);
    let newLevel = blankLevel
      |> Level.Level.modifyTile(1, 0, { tile: GROUND, state: Enemy(active) })
      |> Level.Level.modifyTile(10, 10, { tile: GROUND, state: Enemy(quickEnemy("2", 0.5)) });

    let newWorld = World.World.updateLevel(newLevel, game.world);

    let initGame = { ... game, world: newWorld};
    let postLoop = EnemyLoop.takeTurn({ enemy: active, location: (1, 0) }, newLevel, initGame);

    test("Does not change the current level", (_) => {
      let level = postLoop >>= (game => World.World.currentLevel(game.world));
      expect(Rationale.Option.isSome(level)) |> toBe(true);
    });

    test("Resets any sleeping enemies position", (_) => {
      let activeEnemies = postLoop >>= (game => World.World.currentLevel(game.world))
        |> Rationale.Option.fmap(l => EnemyLoop.findActiveEnemies(l.map))
        |> Rationale.Option.fmap(List.length)
        |> Rationale.Option.default(99);

      expect(activeEnemies) |> toBe(0);
    });
  });

  describe("can attack", () => {
    test("returns true when the player is in range", (_) => {
      let (_, level) = gameWithAttackablePlayer(game);
      let canAttack = EnemyLoop.canAttack(level.map, { enemy: activeEnemy, location: (6, 7)} );

      expect(canAttack) |> toBe(true);
    });

    test("and false true when the player is not in range", (_) => {
      let (_, level) = gameWithAttackablePlayer(game);
      let canAttack = EnemyLoop.canAttack(level.map, { enemy: activeEnemy, location: (3, 3)} );

      expect(canAttack) |> toBe(false);
    });
  });

  describe("find targets", () => {
    
    test("finds targets around the user", (_) => {
      let (_, level) = gameWithEnemyDelta(0, 1, game);
      let enemyInfo = { enemy: activeEnemy, location: (6, 7) }
      let visiblePlaces = EnemyLoop.findTargets(~range=1, enemyInfo);

      expect(List.length(visiblePlaces)) |> toBe(8);
    });

    test("finds more targets when a larger range is used", (_) => {
      let (_, level) = gameWithEnemyDelta(0, 1, game);
      let enemyInfo = { enemy: activeEnemy, location: (6, 7) }
      let visiblePlaces = EnemyLoop.findTargets(~range=2, enemyInfo);

      expect(List.length(visiblePlaces)) |> toBe(24);
    });
  });

  describe("chase", () => {

    test("move north towards the player when the player is in range", (_) => {
      let (_, level) = gameWithEnemyDelta(0, -1, game);
      let result = EnemyLoop.chase(level.map, { enemy: activeEnemy, location: (6, 5) });

      expect(result) |> toEqual((0, 1));
    });

    test("move west towards the player when the player is in range", (_) => {
      let (_, level) = gameWithEnemyDelta(4, 0, game);
      let result = EnemyLoop.chase(level.map, { enemy: activeEnemy, location: (10, 6) });

      expect(result) |> toEqual((-1, 0));
    });

    test("move south east towards the player when the player is in range", (_) => {
      let (_, level) = gameWithEnemyDelta(-1, 1, game);
      let result = EnemyLoop.chase(level.map, { enemy: activeEnemy, location: (5, 7) });

      expect(result) |> toEqual((1, -1));
    });

    test("does not move when the player is out of range", (_) => {
      let (_, level) = gameWithEnemyDelta(6, 6, game);
      let result = EnemyLoop.chase(level.map, { enemy: activeEnemy, location: (12, 12)} );

      expect(result) |> toEqual((0, 0));
    });
  });
});
