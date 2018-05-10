open Jest;
open Level;
open Types;
open Rationale;
open Loop;

let defaultPlayer = { name:"default", stats: { health: 1, speed: 1.0, position: -1.}, gold: 5, location: (-9, -9) };
let blankWorld = LevelBuilder.makeBlankWorld("test");
let playerAt = (x, y) => PLAYER({name:"test", stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (x, y)});

let modifiedLevel =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: playerAt(0, 0) });

describe("Loop.update", () => {

  test("Leaves the user active", (_) => {
    open Expect;

    let updatedMap = Loop.update(modifiedLevel.map);
    
    let updatedPlayer = Area.findPlayer(updatedMap) |> Option.default(defaultPlayer);
    
    expect(updatedPlayer.stats.position) |> toBeGreaterThan(0.);
  });
});
