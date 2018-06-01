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

    test("Resets the active enemies position", (_) => {
      let activeEnemies = postLoop >>= (game => World.World.currentLevel(game.world))
        |> Rationale.Option.fmap(l => EnemyLoop.findActiveEnemies(l.map))
        |> Rationale.Option.fmap(List.length)
        |> Rationale.Option.default(99);

      expect(activeEnemies) |> toBe(0);
    });
  });
});