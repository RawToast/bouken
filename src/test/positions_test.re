open Level;
open Types;
open Positions;

open Jest;
open Expect;

let blankWorld = LevelBuilder.makeBlankLevel("test");

let basicStats = {health: 10, speed: 1.0, position: 0.0};

let defaultEnemy = {
  id: "default",
  name: "default",
  stats: {
    health: 1,
    speed: 1.0,
    position: (-1.),
  },
};

let defaultPlayer = {
  name: "default",
  stats: {
    health: 1,
    speed: 1.0,
    position: (-1.),
  },
  gold: 5,
  location: ((-9), (-9)),
};

let playerAt = (x, y) => Player({name: "test", stats: basicStats, gold: 5, location: (x, y)});

let enemy = Enemy({id: "enemy", name: "enemy", stats: basicStats});

let modifiedLevel =
  blankWorld
  |> Level.modifyTile(0, 0, {tile: GROUND, state: playerAt(0, 0)})
  |> Level.modifyTile(5, 5, {tile: GROUND, state: enemy});

describe("BasicPositions", () =>
    describe("incrementAll", () => {
      let updatedMap = BasicPositions.incrementAll(modifiedLevel.map);
      test("Retains the player", (_) => {
        let updatedPlayer = Area.findPlayer(updatedMap);
        expect(Rationale.Option.isSome(updatedPlayer)) |> toBe(true);
      });
      test("Increments the player's position", (_) => {
        let updatedPlayer = Area.findPlayer(updatedMap) |> Rationale.Option.default(defaultPlayer);
        expect(updatedPlayer.stats.position) |> toBeGreaterThanOrEqual(defaultPlayer.stats.position);
      });
      test("Increments the enemys positsion", (_) => {
        let maybeEnemy = Area.findEnemy("enemy", updatedMap);
        let updatedEnemy: enemy = Rationale.Option.default(defaultEnemy, maybeEnemy);
        let position: float = updatedEnemy.stats.position;
        expect(position) |> toBeGreaterThanOrEqual(defaultEnemy.stats.position);
      });
    })
);
