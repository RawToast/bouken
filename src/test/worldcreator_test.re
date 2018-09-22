open Types;
open Jest;
open Expect;
open Worldcreator.CsvWorldBuilder;

let initEnemy = Enemy.Enemies.makeEnemy();

describe("buildPlace", () => {
  describe("When given '.'", () => {
    let result = buildPlace(".");

    test("Builds a ground tile", (_) => expect(result.tile) |> toBe(GROUND));
    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given '#'", () => {
    let result = buildPlace("#");

    test("Builds a wall", (_) => expect(result.tile) |> toBe(WALL));
    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given ':'", () => {
    let result = buildPlace(":");

    test("Builds a rough tile", (_) => expect(result.tile) |> toBe(ROUGH));
    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given 'w'", () => {
    let result = buildPlace("w");

    test("Builds a water tile", (_) => expect(result.tile) |> toBe(WATER));
    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given 'e50'", () => {
    let result = buildPlace("e50");

    test("Builds an exit tile", (_) => expect(result.tile |> Level.Tiles.isExit) |> toBe(true));
    test("Sets the score", (_) => expect(result.tile) |> toEqual(EXIT(50)));

    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given '/0Swamp'", () => {
    let result = buildPlace("/0Swamp");

    test("Builds a Stair tile", (_) => expect(result |> Level.Tiles.isStairs) |> toBe(true));
    test("Linking to the Swap", (_) => expect(result.tile) |> toEqual(STAIRS({ id: 0, level: "Swamp"})));

    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given '.|Z'", () => {
    let result = buildPlace(".|Z");

    test("Builds a ground tile", (_) => expect(result.tile) |> toEqual(GROUND));
    test("Occupied by an enemy", (_) => expect(result |> Level.Tiles.isEnemy) |> toBe(true));
  });

  describe("When given '.|+'", () => {
    let result = buildPlace(".|+");

    test("Builds a ground tile", (_) => expect(result.tile) |> toEqual(GROUND));
    test("Containing health", (_) => expect(result |> Level.Tiles.isHeal) |> toBe(true));
  });

  describe("When given '.|:'", () => {
    let result = buildPlace(".|:");

    test("Builds a ground tile", (_) => expect(result.tile) |> toEqual(GROUND));
    test("Containing an object", (_) => expect(result |> Level.Tiles.hasEffect) |> toBe(true));
  });
  
  describe("When given '.|g'", () => {
    let result = buildPlace(".|g");

    test("Builds a ground tile", (_) => expect(result.tile) |> toEqual(GROUND));
    test("Containing gold", (_) => expect(result |> Level.Tiles.isGold) |> toBe(true));
  });

  describe("When given '.|;'", () => {
    let result = buildPlace(".|;");

    test("Builds a ground tile", (_) => expect(result.tile) |> toEqual(GROUND));
    test("Containing a snare", (_) => expect(result |> Level.Tiles.isSnare) |> toBe(true));
  })
});

describe("buildArea", () => {
  describe("When given a valid string for a 5x5 level", () => {
    open Rationale;
    let levelStr = 
      "., ., ., ., e20\n" ++ 
      "., ., ., .|Z, .\n" ++
      "., w, ., /0Maze, .\n" ++
      "., ., #, ., .\n" ++
      "., ., ., ., .";

    let result = buildArea(levelStr);
    let getPlace = (x, y, area) => area |> List.nth(_, y) |> List.nth(_, x);

    test("Creates an area with 5 rows", (_) => expect(List.length(result)) |> toBe(5));
    test("Creates an area with 5 columns", (_) => expect(result |> List.hd |> List.length) |> toBe(5));

    test("Creates a water tile", (_) => expect(result |> List.flatten |> RList.any(p => p.tile == WATER)) |> toBe(true));
    test("Creates a wall tile", (_) => expect(result |> List.flatten |> RList.any(p => p.tile == WALL)) |> toBe(true));
    test("Creates an exit tile", (_) => expect(result |> List.flatten |> RList.any(p => p.tile == EXIT(20))) |> toBe(true));

    test("Creates a place with an enemy", (_) => expect(result |> List.flatten |> RList.any(Level.Tiles.isEnemy)) |> toBe(true));
    test("Creates a place with srairs", (_) => expect(result |> List.flatten |> RList.any(Level.Tiles.isStairs)) |> toBe(true));

    test("Places the water in the right location", (_) => expect(result |> getPlace(1, 2) |> p => p.tile) |> toBe(WATER));
    test("Places the wall in the right location", (_) => expect(result |> getPlace(2, 1) |> p => p.tile) |> toBe(WALL));
  });
});

describe("buildLevel", () => {
  describe("When given a valid area and name", () => {
    let levelStr = 
      "., ., ., ., e20\n" ++ 
      "., ., ., .|Z, .\n" ++
      "., w, ., /0Maze, .\n" ++
      "., ., #, ., .\n" ++
      "., ., ., .|+, .";

    let result = buildLevel("Test", levelStr);

    test("Creates a named level", (_) => expect(result.name) |> toBe("Test"));
    test("Creates an area with the correct number of rows", (_) => expect(List.length(result.map)) |> toBe(5));
    test("Creates an area with the correct number of columns", (_) => expect(result.map |> List.hd |> List.length) |> toBe(5));
  });
});
