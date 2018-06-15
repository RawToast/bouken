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

let walledWorld = 
"., ., ., ., ., .
., ., ., #, ., .
., #, ., #, ., .
., #, ., #, ., .
., #, ., ., ., .
., #, ., #, ., .";

let simpleWorld = 
"., ., .,
 ., #, .
 ., ., .";

let level = CsvWorldBuilder.buildArea(blankGrid);
let walledLevel = CsvWorldBuilder.buildArea(walledWorld);
let simpleLevel = CsvWorldBuilder.buildArea(simpleWorld);

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
  });

  describe("findRoute", () => {
    let routes1 = findRoute(~limit=4, level, (0, 0), (4, 4));

    let routes2 = findRoute(~limit=4, walledLevel, (0, 0), (0, 4));

    test("Is successful when a path exists 1", (_) =>
      expect(routes1 |> List.length) |> toBeGreaterThanOrEqual(1)
    );

    test("Can find a route with walls", (_) =>
      expect(routes2 |> List.length) |> toBeGreaterThanOrEqual(1)
    );
  });

  describe("findRoutes", () => {
    let routes1: list(list((int, int))) = findRoutes(~limit=4, level, (1, 1), (5, 5));
    let routes2: list(list((int, int))) = findRoutes(~limit=4, level, (2, 0), (2, 4));
    let walledRoutes = findRoutes(~limit=4, walledLevel, (0, 0), (0, 4));

    let moreRoutes: list(list((int, int))) = findRoutes(~limit=3, simpleLevel, (0, 0), (2, 2));
    let manyRoutes: list(list((int, int))) = findRoutes(~limit=4, simpleLevel, (0, 0), (2, 2));

    /* let toStr = (xy) => { let (x, y) = xy; "(" ++ string_of_int(x) ++ ", " ++ string_of_int(y) ++ ")"};
    let routeAsString = l => List.fold_right((ints, s) => if(Js.String.length(s) > 0) {s ++ ":" ++ toStr(ints)} else toStr(ints), l, "");
    let bigStr = (ll) => ll |> List.map(routeAsString);

    manyRoutes |> bigStr |> List.iter(Js.Console.log); */
    
    test("Finds a single route when only one exists", (_) => expect(routes1 |> List.length) |> toBe(1));

    test("Finds many routes when possible possible routes", (_) => expect(routes2 |> List.length) |> toBe(19));

    test("Can find a route with walls", (_) => expect(walledRoutes |> List.length) |> toBe(1));

    test("Finds exactly two routes when only two are possible", (_) => expect(moreRoutes |> List.length) |> toBe(2));

    test("Finds exactly 8 routes when only two are possible", (_) => expect(manyRoutes |> List.length) |> toBe(8));
  });
  }
);
