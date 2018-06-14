open Jest;
open Expect;
open Worldcreator; 
open Pathing;

module Game = Modules.Game;

let blankGrid = 
"., ., ., ., ., .,\n
., ., ., ., ., .,\n
., ., ., ., ., .,\n
., ., ., ., ., .,\n
., ., ., ., ., .,\n
., ., ., ., ., .,";

let level = CsvWorldBuilder.buildArea(blankGrid);

describe("Pathing", () =>
  describe("canNavigateTo", () => {

    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (4, 4))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (0, 4))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (4, 0))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (2, 4))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (4, 3))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (4, 4), (0, 0))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (4, 4), (0, 5))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (4, 4), (6, 2))) |> toBe(true)
    );

    test("Fails when the goal is out of range", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (5, 5))) |> toBe(false)
    );

    test("Cannot move to a position that is out of bounds", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (-1, -1))) |> toBe(false)
    );
  })
);