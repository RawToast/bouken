open Jest;
open Expect;
open Worldcreator; 

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

    let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
      let rec navigate = ((x, y), turn) => {
        if (turn > limit) false
        else if(x == tx && y == ty) true
        else navigate((x - 1, y + 1), turn + 1) || navigate((x, y + 1), turn + 1) || navigate((x + 1, y + 1), turn + 1) 
          || navigate((x - 1 , y), turn + 1) || navigate((x + 1, y), turn + 1)
          || navigate((x - 1, y - 1), turn + 1) || navigate((x, y - 1), turn + 1) || navigate((x + 1, y - 1), turn + 1)
      };

      navigate((x, y), 0);
    };

    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0,0), (4, 4))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0,0), (0, 4))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0,0), (4, 0))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0,0), (2, 4))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (0,0), (4, 3))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (4,4), (0, 0))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (4,4), (0, 5))) |> toBe(true)
    );
    test("Is successful when a path exists", (_) =>
      expect(canNavigateTo(~limit=4, level, (4,4), (6, 2))) |> toBe(true)
    );
  })
);