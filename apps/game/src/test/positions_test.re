module LevelBuilder = Level.LevelBuilder;
module Area = Level.Area;
module Level = Level.Level;
open Types;
open Positions;

open Jest;
open Expect;

let blankWorld = LevelBuilder.makeBlankLevel("test");

let basicStats = {health: 10, speed: 1.0, position: 0.0, damage: 1};

let defaultEnemy = {
  id: "default",
  name: "default",
  stats: {
    health: 1,
    speed: 1.0,
    position: (-1.),
    damage: 1
  },
  ai: { moveRange: 4, terrainCost: false, mustSee: true, memory: None, attackRange: 1, flying: false, swim: false, seedark: false, small: false }
};

let defaultPlayer = {
  name: "default",
  stats: {
    health: 1,
    speed: 1.0,
    position: (-1.),
    damage: 1
  },
  gold: 5,
  location: ((-9), (-9)),
};

let playerAt = (x, y) => Player({name: "test", stats: basicStats, gold: 5, location: (x, y)});

let enemy = Enemy({id: "enemy", name: "enemy", stats: basicStats, ai: { moveRange: 4, terrainCost: false, mustSee: true, memory: None, attackRange: 1, flying: false, swim: false, seedark: false, small: false }});

let modifiedLevel =
  blankWorld
  |> Level.modifyTile(0, 0, {tile: GROUND, state: playerAt(0, 0), tileEffect: NoEff, visible: false })
  |> Level.modifyTile(5, 5, {tile: GROUND, state: enemy, tileEffect: NoEff, visible: false });

describe("BasicPositions", () =>
    describe("incrementAll", () => {
      let updatedMap = BasicPositions.incrementAll(modifiedLevel.map);
      test("Retains the player", (_) => {
        let updatedPlayer = Area.findPlayer(updatedMap);
        expect(Belt_Option.isSome(updatedPlayer)) |> toBe(true);
      });
      test("Increments the player's position", (_) => {
        let updatedPlayer = Area.findPlayer(updatedMap) -> Belt_Option.getWithDefault(defaultPlayer);
        expect(updatedPlayer.stats.position) |> toBeGreaterThanOrEqual(defaultPlayer.stats.position);
      });
      test("Increments the enemys positsion", (_) => {
        let maybeEnemy = Area.findEnemy("enemy", updatedMap);
        let updatedEnemy: enemy = Belt_Option.getWithDefault(maybeEnemy, defaultEnemy);
        let position: float = updatedEnemy.stats.position;
        expect(position) |> toBeGreaterThanOrEqual(defaultEnemy.stats.position);
      });
    })
);
