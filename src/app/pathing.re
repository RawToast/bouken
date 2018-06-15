open Types;

let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
  let maxX = List.length(List.hd(area)) - 1;
  let maxY = List.length(area) - 1;

  let rec navigate = ((x, y), turn) => {
    if (turn > limit) false
    else if(x < 0 || y < 0) false
    else if(x > maxX || y > maxY) false
    else if(area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupy == false) false
    else if(x == tx && y == ty) true
    else navigate((x - 1, y + 1), turn + 1) || navigate((x, y + 1), turn + 1) || navigate((x + 1, y + 1), turn + 1) 
      || navigate((x - 1 , y), turn + 1) || navigate((x + 1, y), turn + 1)
      || navigate((x - 1, y - 1), turn + 1) || navigate((x, y - 1), turn + 1) || navigate((x + 1, y - 1), turn + 1)
  };

  navigate((x, y), 0);
};

let findRoute = (~limit=4, area, (x, y), (tx, ty)) => {
  let maxX = List.length(List.hd(area)) - 1;
  let maxY = List.length(area) - 1;

  let rec recRoutes = ((x, y), turn, routes) => {
    if (turn > limit) []
    else if(x < 0 || y < 0) []
    else if(x > maxX || y > maxY) []
    else if(area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupy == false) []
    else if(x == tx && y == ty) routes
    else {
      let nxt: list((int, int)) = [ (x, y), ... routes ];
      recRoutes((x - 1, y + 1), turn + 1, nxt) @ recRoutes((x, y + 1), turn + 1, nxt) @ recRoutes((x + 1, y + 1), turn + 1, nxt)
      @ recRoutes((x - 1 , y), turn + 1, nxt) @ recRoutes((x + 1, y), turn + 1, nxt)
      @ recRoutes((x - 1, y - 1), turn + 1, nxt) @ recRoutes((x, y - 1), turn + 1, nxt) @ recRoutes((x + 1, y - 1), turn + 1, nxt)
    }
  };

  recRoutes((x, y), 0, []);
};

let findRoutes = (~limit=4, area, (x, y), (tx, ty)) => {
  let maxX = List.length(List.hd(area)) - 1;
  let maxY = List.length(area) - 1;

  let rec recRoutes = ((x, y), turn, current, routes) => {
    if (turn > limit) routes
    else if (x < 0 || y < 0) routes
    else if (x > maxX || y > maxY) routes
    else if (area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupy == false) routes
    else if (x == tx && y == ty) [ [(x, y), ...current], ... routes ]
    else {
      let history = if (turn == 0) { current }  else { [ (x, y), ... current ] };
    
      recRoutes((x - 1, y + 1), turn + 1, history, routes) @ recRoutes((x, y + 1), turn + 1, history, routes) @ recRoutes((x + 1, y + 1), turn + 1, history, routes)
      @ recRoutes((x - 1 , y), turn + 1, history, routes) @ recRoutes((x + 1, y), turn + 1, history, routes)
      @ recRoutes((x - 1, y - 1), turn + 1, history, routes) @ recRoutes((x, y - 1), turn + 1, history, routes) @ recRoutes((x + 1, y - 1), turn + 1, history, routes)
    }
  };

  recRoutes((x, y), 0, [], []);
};
type coord = { x: int, y: int};

let findFastestRoutes = (~limit=4, area, (x, y), (tx, ty)) => {
  let maxX = List.length(List.hd(area)) - 1;
  let maxY = List.length(area) - 1;
  let zl = (0,0);
  let countPenalties = locations => locations
    |> List.map(loc => { let (x, y) = loc; area |> List.nth(_, y) |> List.nth(_, x) |>  Level.Tiles.placePenalty});
  
  let rec recRoutes = ((x, y), turn, best, current, routes) => {
    if (turn > limit) routes
    else if (x < 0 || y < 0) routes
    else if (x > maxX || y > maxY) routes
    else if (List.length(current) > best) routes
    else if (area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupy == false) routes
    else if (x == tx && y == ty) {
      let allPaths = [[(x, y), ...current], ... routes ];
      let historys = List.fold_left(
          (r1, r2) => if (countPenalties(r2) > countPenalties(r1)) r1 else r2, 
          [zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl,zl], 
          allPaths);
      [historys];
    }
    else {
      let history = if (turn == 0) current else [ (x, y), ... current ];
      let lowest = if (List.length(routes) == 0) best else routes |> List.hd |> List.length;
      recRoutes((x - 1, y + 1), turn + 1, lowest, history, routes) 
        @ recRoutes((x, y + 1), turn + 1, lowest, history, routes)
        @ recRoutes((x + 1, y + 1), turn + 1, lowest, history, routes)
        @ recRoutes((x - 1 , y), turn + 1, lowest, history, routes) 
        @ recRoutes((x + 1, y), turn + 1, lowest, history, routes)
        @ recRoutes((x - 1, y - 1), turn + 1, lowest, history, routes) 
        @ recRoutes((x, y - 1), turn + 1, lowest, history, routes) 
        @ recRoutes((x + 1, y - 1), turn + 1, lowest, history, routes)
    }
  };

  recRoutes((x, y), 0, 20, [], []);
};
