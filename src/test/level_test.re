open Jest;
open Level;
open Types;
open Rationale;

let blankWorld = LevelBuilder.makeBlankWorld("test");

describe("Level.modify", () => {
  test("Can modify a tile", (_) => {
    open Expect;
    let modifiedLevel =
      blankWorld
      |> Level.modifyTile(0, 0, {tile: WATER, state: EMPTY});
    
      expect(modifiedLevel.map |> List.hd |> List.hd |> (i => i.tile)) |> toBe(WATER);
  });
});

describe("Level.findPlayer", () => {
  test("Finds the player", (_) => {
    open Expect;
    let player =
      blankWorld
      |> Level.modifyTile(0, 0, {tile: GROUND, state: PLAYER({name:"test", health: 10, gold: 5, location: (0, 0)})}) 
      |> Level.findPlayer;
    
      expect(Rationale.Option.isSome(player)) |> toEqual(true);
  });

  test("Returns empty when there is no player", (_) => {
    open Expect;
    let player = blankWorld |> Level.findPlayer;
    
      expect(Rationale.Option.isNone(player)) |> toEqual(true);
  });
});

describe("Level.movePlayer", () => {
  test("Moves the player when the destination is valid", (_) => {
    open Expect;
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: PLAYER({ name:"test", health: 10, gold: 5, location: (0, 0) }) }) 
      |> Level.movePlayer(0, 1);
    
      expect(Result.isOk(result)) |> toEqual(true);
  });

  test("Returns an error when there is no player", (_) => {
    open Expect;
    let result =
      Level.movePlayer(0, 1, blankWorld);
    
      expect(Result.isError(result)) |> toEqual(true);
      
  });

  test("Returns an error when the target location is a wall", (_) => {
    open Expect;
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: PLAYER({ name:"test", health: 10, gold: 5, location: (0, 0) }) }) 
      |> Level.modifyTile(0, 1, { tile: WALL, state: EMPTY }) 
      |> Level.movePlayer(0, 1);
    
      expect(Result.isError(result)) |> toEqual(true);
  });

  test("Moves the player can move when the target is water", (_) => {
    open Expect;
    let result =
      blankWorld
      |> Level.modifyTile(0, 0, { tile: GROUND, state: PLAYER({ name:"test", health: 10, gold: 5, location: (0, 0) }) }) 
      |> Level.modifyTile(0, 1, { tile: WATER, state: EMPTY }) 
      |> Level.movePlayer(0, 1);
    
      expect(Result.isOk(result)) |> toEqual(true);
  });
});
