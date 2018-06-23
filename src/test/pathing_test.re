open Jest;
open Expect;
open Worldcreator; 
open Pathing;

module Game = Modules.Game;
let blankGrid = 
"., ., ., ., ., .
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

let bigBlankGrid = 
  "., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., ., ., ., ., ., .";

let level = CsvWorldBuilder.buildArea(blankGrid);
let walledLevel = CsvWorldBuilder.buildArea(walledWorld);
let simpleLevel = CsvWorldBuilder.buildArea(simpleWorld);
let bigWorld = CsvWorldBuilder.buildArea(bigBlankGrid);

describe("Pathing", () => {

  test("Max X is 6", (_) =>
    expect(List.length(level)) |> toBe(6)
  );

  test("Max Y is 6", (_) =>
    expect(level |> List.hd |> List.length) |> toBe(6)
  );

  describe("canNavigateTo", () => {

    test("Is successful when a path exists 1", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (4, 4))) |> toBe(true));

    test("Is successful when a path exists 2", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (0, 4))) |> toBe(true));

    test("Is successful when a path exists 3", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (4, 0))) |> toBe(true));

    test("Is successful when a path exists 4", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (2, 4))) |> toBe(true));

    test("Is successful when a path exists 5", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (4, 3))) |> toBe(true));

    test("Is successful when a path exists 6", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (4, 4), (0, 0))) |> toBe(true));

    test("Is successful when a path exists 7", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (4, 4), (0, 5))) |> toBe(true));

    test("Is successful when a path exists 8", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (4, 4), (5, 2))) |> toBe(true));

    test("Fails when the goal is out of range", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (5, 5))) |> toBe(false));

    test("Cannot move to a position that is out of bounds 1", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (0, 0), (-1, -1))) |> toBe(false));

    test("Cannot move to a position that is out of bounds 2", (_) =>
      expect(Navigation.canNavigateTo(~limit=4, level, (5, 5), (9, 9))) |> toBe(false));

    test("Cannot move through a wall", (_) =>
      expect(Navigation.canNavigateTo(~limit=2, walledLevel, (0, 0), (2, 0))) |> toBe(false));

    test("Can move around a wall", (_) =>
      expect(Navigation.canNavigateTo(~limit=8, walledLevel, (0, 0), (2, 0))) |> toBe(true));
  });

  describe("findRoute", () => {
    let routes1 = PathUtil.findRoute(~limit=4, level, (0, 0), (4, 4));

    let routes2 = PathUtil.findRoute(~limit=4, walledLevel, (0, 0), (0, 4));

    test("Is successful when a path exists 1", (_) =>
      expect(routes1 |> List.length) |> toBeGreaterThanOrEqual(1)
    );

    test("Can find a route with walls", (_) =>
      expect(routes2 |> List.length) |> toBeGreaterThanOrEqual(1)
    );
  });

  describe("findRoutes", () => {
    let routes1: list(list((int, int))) = PathUtil.findRoutes(~limit=4, level, (1, 1), (5, 5));
    let routes2: list(list((int, int))) = PathUtil.findRoutes(~limit=4, level, (2, 0), (2, 4));
    let walledRoutes = PathUtil.findRoutes(~limit=4, walledLevel, (0, 0), (0, 4));

    let twoRoutes: list(list((int, int))) = PathUtil.findRoutes(~limit=3, simpleLevel, (0, 0), (2, 2));
    let manyRoutes: list(list((int, int))) = PathUtil.findRoutes(~limit=4, simpleLevel, (0, 0), (2, 2));
    
    test("Finds a single route when only one exists", 
      (_) => expect(routes1 |> List.length) |> toBe(1));

    test("Finds many routes when possible possible routes", 
      (_) => expect(routes2 |> List.length) |> toBe(19));

    test("Can find a route with walls", 
      (_) => expect(walledRoutes |> List.length) |> toBe(1));

    test("Finds exactly two routes when only two are possible", 
      (_) => expect(twoRoutes |> List.length) |> toBe(2));

    test("Finds exactly 8 routes when only two are possible", 
      (_) => expect(manyRoutes |> List.length) |> toBe(8));
  });

  describe("findFastestRoutes", () => {
    let routes1: list(list((int, int))) = PathUtil.findFastestRoutes(~limit=4, level, (1, 1), (5, 5));
    let routes2: list(list((int, int))) = PathUtil.findFastestRoutes(~limit=4, level, (2, 0), (2, 4));
    let walledRoutes = PathUtil.findFastestRoutes(~limit=4, walledLevel, (0, 0), (0, 4));

    let twoRoutes: list(list((int, int))) = PathUtil.findFastestRoutes(~limit=3, simpleLevel, (0, 0), (2, 2));
    let manyRoutes: list(list((int, int))) = PathUtil.findFastestRoutes(~limit=4, simpleLevel, (0, 0), (2, 2));
    
    let oneMove =  PathUtil.findFastestRoutes(~limit=4, level, (2, 0), (2, 1));

    test("Finds a single route when only one exists", 
      (_) => expect(routes1 |> List.length) |> toBe(1));

    test("Finds many routes when possible possible routes", 
      (_) => expect(routes2 |> List.length) |> toBe(19));

    test("Can find a route with walls", 
      (_) => expect(walledRoutes |> List.length) |> toBe(1));

    test("Finds exactly two routes when there are two equal routes", 
      (_) => expect(twoRoutes |> List.length) |> toBe(2));

    test("Finds only the fastest routes when there are routes of different length", 
      (_) => expect(manyRoutes |> List.length) |> toBe(2));

    test("Suggests the only one possible route", 
      (_) => expect(List.length(oneMove)) |> toBe(1));

    test("Suggests the only possible move", 
      (_) => expect(oneMove |> List.hd |> List.length) |> toBe(1));

    test("Suggests the correct possible move", 
      (_) => expect(List.hd(List.hd(oneMove))) |> toEqual((2, 1)));

    let oneCorrectMove =  PathUtil.findFastestRoutes(~limit=4, bigWorld, (2, 0), (2, 1));

    test("Suggests the best possible route", 
      (_) => expect(List.length(oneCorrectMove)) |> toBe(1));
  });

  describe("suggestMove", () => {
    let impossibleMove = Navigation.suggestMove(~limit=4, level, (1, 1), (6, 6));
    let possibleMove =  Navigation.suggestMove(~limit=4, level, (2, 0), (2, 4));
    let expectedMove = Navigation.suggestMove(~limit=4, walledLevel, (0, 0), (0, 4));
    let oneMove =  Navigation.suggestMove(~limit=4, level, (2, 0), (2, 1));
    let bestMove = Navigation.suggestMove(~limit=4, bigWorld, (5, 11), (3, 9));

    test("Suggests to stay when no move is available", 
      (_) => expect(impossibleMove) |> toEqual((0, 0)));
    
    test("Suggests to move when possible", 
      (_) => expect(possibleMove) |> not_ |> toEqual((0, 0)));

    test("Suggests the only possible move", 
      (_) => expect(expectedMove) |> toEqual((0, 1)));
    
    test("Suggests the only possible move", 
      (_) => expect(oneMove) |> toEqual((0, 1)));
    
    test("Suggests to move into range", 
      (_) => expect(bestMove) |> toEqual((-1, -1)));
    });

  let smallGrid = 
  "., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., .
  ., ., #, ., ., ., ., .
  ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., .
  ., ., ., ., ., ., ., .";

  let smallLevel = CsvWorldBuilder.buildArea(blankGrid);

  describe("canSee", () => {
    let canSee = VisionUtil.canSee(~limit=4, smallLevel, (2, 1));

    test("Can see a tile that is infront of the origin", 
      (_) => expect(canSee((2,2))) |> toEqual(true));
    test("Can see a tile that is behind the origin", 
      (_) => expect(canSee((2,0))) |> toEqual(true));
    test("Can see a tile that is to the left of the origin", 
      (_) => expect(canSee((0,1))) |> toEqual(true));
    test("Can see a tile that is to the right of the origin", 
      (_) => expect(canSee((4, 1))) |> toEqual(true));
    test("Can a tile that is diagonal to the origin", 
      (_) => expect(canSee((4, 3))) |> toEqual(true));
    test("Can a tile that somewhat diagonal of the origin", 
      (_) => expect(canSee((3, 3))) |> toEqual(true));
    test("Can a tile that somewhat diagonal of the origin 2", 
      (_) => expect(canSee((4, 5))) |> toEqual(true));

    test("Cannot see a tile that is behind a wall", 
      (_) => expect(canSee((2, 5))) |> toEqual(false));
    test("Cannot see a tile that is out of range", 
      (_) => expect(canSee((7, 1))) |> toEqual(false));
  });
});
