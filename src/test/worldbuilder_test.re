open Types;
open Jest;
open Expect;
open Worldbuilder;

let initEnemy = {id: "testenemy", name: "spooky thing", stats: { health: 3, speed: 1.0, position: 0. }};

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

  describe("When given 'w'", () => {
    let result = buildPlace("w");

    test("Builds a water tile", (_) => expect(result.tile) |> toBe(WATER));
    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });

  describe("When given 'e50'", () => {
    let result = buildPlace("e50");

    test("Builds an exit tile", (_) => expect(result.tile) |> toEqual(EXIT(50)));
    test("The place is empty", (_) => expect(result.state) |> toBe(Empty));
  });
});

describe("buildLevel", () => {
  describe("When given a valid string for a 5x5 level", () => {
    let levelStr = 
      "., ., ., ., e20\n" ++ 
      "., ., ., ., .\n" ++
      "., w, ., ., .\n" ++
      "., ., #, ., .\n" ++
      "., ., ., ., .";

    let result = buildArea(levelStr);
    let getPlace = (x, y, area) => area |> List.nth(_, y) |> List.nth(_, x);

    test("Creates an area with 5 rows", (_) => expect(List.length(result)) |> toBe(5));

    test("Creates an area with 5 columns", (_) => expect(result |> List.hd |> List.length) |> toBe(5));

    test("Creates a water tile", (_) => expect(result |> List.flatten |> Rationale.RList.any(p => p.tile == WATER)) |> toBe(true));

    test("Creates a wall tile", (_) => expect(result |> List.flatten |> Rationale.RList.any(p => p.tile == WALL)) |> toBe(true));

    test("Creates an exit tile", (_) => expect(result |> List.flatten |> Rationale.RList.any(p => p.tile == EXIT(20))) |> toBe(true));

    test("Places the water in the right location", (_) => expect(result |> getPlace(1, 2) |> p => p.tile) |> toBe(WATER));

    test("Places the wall in the right location", (_) => expect(result |> getPlace(2, 1) |> p => p.tile) |> toBe(WALL));
  });
});
