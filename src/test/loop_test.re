open Jest;
open Level;
open Types;
open Rationale;
open Loop;

let blankWorld = LevelBuilder.makeBlankWorld("test");

let basicStats = { health: 10, speed: 1.0, position: 0.0 };
let defaultEnemy = { id: "default", name:"default", stats: { health: 1, speed: 1.0, position: -1.} };
let defaultPlayer = { name:"default", stats: { health: 1, speed: 1.0, position: -1.}, gold: 5, location: (-9, -9) };
let playerAt = (x, y) => PLAYER({ name:"test", stats: basicStats, gold: 5, location: (x, y) });
let enemy = ENEMY({ id: "enemy", name:"enemy", stats: basicStats });

let modifiedLevel =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0) })
      |> Level.modifyTile(5, 5, { tile: GROUND, state: enemy });

describe("Loop", () => {
  open Expect;

  describe("incrementPositions", () => {
    let updatedMap = Loop.incrementPositions(modifiedLevel.map);

    test("Increments the player's position", (_) => {      
      let updatedPlayer = Area.findPlayer(updatedMap) |> Option.default(defaultPlayer);

      expect(updatedPlayer.stats.position) |> toBeGreaterThanOrEqual(1.);
    });

    /* test("Increments the enemy's position", (_) => {      
      let maybeEnemy = Area.findEnemy("enemy", updatedMap);
      let updatedEnemy: enemy = Option.default(defaultEnemy, maybeEnemy);
      let position = updatedEnemy.stats.position;
      
      expect(position) |> toBe(1.);

      /* expect(updatedEnemy.stats) |> toEqual( { health: 10, speed: 1.0, position: 1. }); */
    }); */
  });

  describe("update", () => {
    let updatedMap: area = Loop.update(modifiedLevel.map);

    test("Leaves the user active", (_) => {
      let updatedPlayer = Area.findPlayer(updatedMap) |> Option.default(defaultPlayer);
      expect(updatedPlayer.stats.position) |> toBeGreaterThanOrEqual(1.);
    });
  });
});
