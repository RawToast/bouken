open Jest;
open Expect;
open Worldcreator; 
open Pathing;

module Game = Modules.Game;

let blankGrid = "., ., ., ., ., .
., ., ., ., ., .
., ., ., ., ., .
., ., ., ., ., .
., ., ., ., ., .
., ., ., ., ., .";

let walledWorld = "., ., ., ., ., .
., ., ., #, ., .
., #, ., #, ., .
., #, ., #, ., .
., #, ., ., ., .
., #, ., #, ., .";

let level = CsvWorldBuilder.buildArea(blankGrid);
let walledLevel = CsvWorldBuilder.buildArea(walledWorld);

describe("Pathing", () => {

  test("Max X is 6", (_) =>
    expect(List.length(level)) |> toBe(6)
  );

  test("Max Y is 6", (_) =>
    expect(level |> List.hd |> List.length) |> toBe(6)
  );

  describe("canNavigateTo", () => {

    test("Is successful when a path exists 1", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (4, 4))) |> toBe(true)
    );
    test("Is successful when a path exists 2", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (0, 4))) |> toBe(true)
    );
    test("Is successful when a path exists 3", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (4, 0))) |> toBe(true)
    );
    test("Is successful when a path exists 4", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (2, 4))) |> toBe(true)
    );
    test("Is successful when a path exists 5", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (4, 3))) |> toBe(true)
    );
    test("Is successful when a path exists 6", (_) =>
      expect(canNavigateTo(~limit=4, level, (4, 4), (0, 0))) |> toBe(true)
    );
    test("Is successful when a path exists 7", (_) =>
      expect(canNavigateTo(~limit=4, level, (4, 4), (0, 5))) |> toBe(true)
    );
    test("Is successful when a path exists 8", (_) =>
      expect(canNavigateTo(~limit=4, level, (4, 4), (5, 2))) |> toBe(true)
    );

    test("Fails when the goal is out of range", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (5, 5))) |> toBe(false)
    );

    test("Cannot move to a position that is out of bounds 1", (_) =>
      expect(canNavigateTo(~limit=4, level, (0, 0), (-1, -1))) |> toBe(false)
    );

    test("Cannot move to a position that is out of bounds 2", (_) =>
      expect(canNavigateTo(~limit=4, level, (5, 5), (9, 9))) |> toBe(false)
    );

    test("Cannot move through a wall", (_) =>
      expect(canNavigateTo(~limit=2, walledLevel, (0, 0), (2, 0))) |> toBe(false)
    );

    test("Can move around a wall", (_) =>
      expect(canNavigateTo(~limit=8, walledLevel, (0, 0), (2, 0))) |> toBe(true)
    );
  })
  }
);