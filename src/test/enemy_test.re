open Types;

open Jest;
open Expect;

module Positions = Positions.BasicPositions;
module EnemyLoop = Enemy.CreateEnemyLoop(Positions, Level.Area, World.World);
module BasicTurnLoop = Gameloop.CreateGameLoop(Positions, EnemyLoop);

describe("EnemyLoop", () => {

    describe("findActiveEnemies", () => {
    
    let activeEnemy = { id: "default", name: "default",
        stats: { health: 1, speed: 1.0, position: (1.) } };

    let level = Level.LevelBuilder.makeBlankLevel("test")
        |> Level.Level.modifyTile(0, 0, { tile: GROUND, state: Enemy(activeEnemy)})
        |> Level.Level.modifyTile(10, 10, { tile: GROUND, state: Enemy({...activeEnemy, id: "alt"})});

    let activeEnemies = EnemyLoop.findActiveEnemies(level.map);

    test("Finds any active enemies", (_) => {
      expect(List.length(activeEnemies)) |> toEqual(2);
    });
  
  });
  
});