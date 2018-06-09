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
});
