open Types;

module PathUtil = {
  let invalidPosition = (x, y) => (x < 0 || y < 0);
  let isOutOfBounds = (x, y, maxX, maxY) => (x > maxX || y > maxY);
  let isInvalidMove = (x, y, area) => (area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupy == false);
  let isGoal = (x, y, tx, ty) => (x == tx && y == ty);

  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let rec navigate = ((x, y), turn) => {
      if (turn > limit) false
      else if(invalidPosition(x, y)) false
      else if(isOutOfBounds(x, y, maxX, maxY)) false
      else if(isInvalidMove(x, y, area)) false
      else if(isGoal(x, y, tx, ty)) true
      else navigate((x - 1, y + 1), turn + 1)
        || navigate((x, y + 1), turn + 1)
        || navigate((x + 1, y + 1), turn + 1)
        || navigate((x - 1 , y), turn + 1) 
        || navigate((x + 1, y), turn + 1)
        || navigate((x - 1, y - 1), turn + 1)
        || navigate((x, y - 1), turn + 1)
        || navigate((x + 1, y - 1), turn + 1)
    };

    navigate((x, y), 0);
  };

  let findRoute = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let rec recRoutes = ((x, y), turn, routes) => {
      if (turn > limit) []
      else if (invalidPosition(x, y)) []
      else if (isOutOfBounds(x, y, maxX, maxY)) []
      else if (isInvalidMove(x, y, area)) []
      else if (isGoal(x, y, tx, ty)) routes
      else {
        let nxt: list((int, int)) = [ (x, y), ... routes ];
        recRoutes((x - 1, y + 1), turn + 1, nxt)
          @ recRoutes((x, y + 1), turn + 1, nxt) 
          @ recRoutes((x + 1, y + 1), turn + 1, nxt)
          @ recRoutes((x - 1 , y), turn + 1, nxt) 
          @ recRoutes((x + 1, y), turn + 1, nxt)
          @ recRoutes((x - 1, y - 1), turn + 1, nxt) 
          @ recRoutes((x, y - 1), turn + 1, nxt)
          @ recRoutes((x + 1, y - 1), turn + 1, nxt)
      }
    };

    recRoutes((x, y), 0, []);
  };

  let findRoutes = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let rec recRoutes = ((x, y), turn, current, routes) => {
      if (turn > limit) routes
      else if (invalidPosition(x, y)) routes
      else if (isOutOfBounds(x, y, maxX, maxY)) routes
      else if (isInvalidMove(x, y, area)) routes
      else if (isGoal(x, y, tx, ty)) [ [(x, y), ...current], ... routes ]
      else {
        let history = if (turn == 0) { current }  else { [ (x, y), ... current ] };
      
        recRoutes((x - 1, y + 1), turn + 1, history, routes)
          @ recRoutes((x, y + 1), turn + 1, history, routes)
          @ recRoutes((x + 1, y + 1), turn + 1, history, routes)
          @ recRoutes((x - 1 , y), turn + 1, history, routes)
          @ recRoutes((x + 1, y), turn + 1, history, routes)
          @ recRoutes((x - 1, y - 1), turn + 1, history, routes)
          @ recRoutes((x, y - 1), turn + 1, history, routes)
          @ recRoutes((x + 1, y - 1), turn + 1, history, routes)
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
      else if (invalidPosition(x, y)) routes
      else if (isOutOfBounds(x, y, maxX, maxY)) routes
      else if (List.length(current) > best) routes
      else if (isInvalidMove(x, y, area)) routes
      else if (isGoal(x, y, tx, ty)) {
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

  let suggestMove = (~limit=4, area, (x, y), (tx, ty)) => {

    (0, 0);
  }
};

module Navigation: Pathfinding = {
  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) =>
    PathUtil.canNavigateTo(~limit=limit, area, (x, y), (tx, ty));

  let suggestMove = (~limit=4, area, (x, y), (tx, ty)) => {
    (0, 0);
  };
};