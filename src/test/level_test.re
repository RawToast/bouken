open Jest;
open Level;
open Types;

describe("Level.modify", () => {

  test("Can modify a tile", (_) => {
    open Expect;
    let modifiedLevel =
      LevelBuilder.makeBlankWorld("test") 
      |> Level.modifyTile(0, 0, {tile: WATER, state: EMPTY});
    
      expect(modifiedLevel.map |> List.hd |> List.hd |> (i => i.tile)) |> toBe(WATER);
  });
});

describe("Level.findPlayer", () => {
  test("Finds the player", (_) => {
    open Expect;
    let player =
      LevelBuilder.makeBlankWorld("test") 
      |> Level.modifyTile(0, 0, {tile: GROUND, state: PLAYER({name:"test", health: 10, gold: 5, location: (0, 0)})}) 
      |> Level.findPlayer;
    
      expect(Rationale.Option.isSome(player)) |> toEqual(true);
  });

  test("Returns empty when there is no player", (_) => {
    open Expect;
    let player =
      LevelBuilder.makeBlankWorld("test") |> Level.findPlayer;
    
      expect(Rationale.Option.isNone(player)) |> toEqual(true);
  });
});

describe("Level.movePlayer", () => {
  test("Moves the player when the destination is valid", (_) => {
    open Expect;
    let player =
      LevelBuilder.makeBlankWorld("test") 
      |> Level.modifyTile(0, 0, { tile: GROUND, state: PLAYER({ name:"test", health: 10, gold: 5, location: (0, 0) }) }) 
      |> Level.movePlayer(0, 1);
    
      expect(Rationale.Option.isSome(player)) |> toEqual(true);
  });
});
