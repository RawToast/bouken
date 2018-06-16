open Types;

module PathUtil = {

  let invalidPosition = (x, y) => (x < 0 || y < 0);
  let isOutOfBounds = (x, y, maxX, maxY) => (x > maxX || y > maxY);
  let isInvalidTerrain = (x, y, area) => (area |> List.nth(_, y) |> List.nth(_, x) |> p => p.tile |> Level.Tiles.isWall == true);

  let isInvalidMove = (x, y, area) => (area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupyOrAttack == false);
  let isGoal = (x, y, tx, ty) => (x == tx && y == ty);

  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let rec navigate = ((x, y), turn, route, canNavi: bool) => {
      if (turn > limit) false
      else if (canNavi) true
      else if (limit - turn < Js.Math.abs_int(tx - x) || limit - turn < Js.Math.abs_int(ty - y)) false
      else if (invalidPosition(x, y)) false
      else if (isOutOfBounds(x, y, maxX, maxY)) false
      else if (isInvalidTerrain(x, y, area)) false
      else if (turn == 1 && isInvalidMove(x, y, area)) false
      else if (isGoal(x, y, tx, ty)) true
      else if (turn != 0 && Rationale.RList.any(xy => {let (ox, oy) = xy; ox == x && oy == y}, route)) false
      else {
        let route = if (turn == 0) { route }  else { [ (x, y), ... route] };

        navigate((x - 1, y + 1), turn + 1, route,
          navigate((x, y + 1), turn + 1, route,
          navigate((x + 1, y + 1), turn + 1, route,
          navigate((x - 1 , y), turn + 1, route,
          navigate((x + 1, y), turn + 1, route,
          navigate((x - 1, y - 1), turn + 1, route,
          navigate((x, y - 1), turn + 1, route,
          navigate((x + 1, y - 1), turn + 1, route, canNavi))))))))
      }
    };

    navigate((x, y), 0, [], false);
  };

  let findRoute = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let rec recRoutes = ((x, y), turn, route) => {
      if (turn > limit) []
      else if (invalidPosition(x, y)) []
      else if (isOutOfBounds(x, y, maxX, maxY)) []
      else if (isInvalidTerrain(x, y, area)) []
      else if (turn == 1 && isInvalidMove(x, y, area)) []
      else if (isGoal(x, y, tx, ty)) route
      else {
        let nxt = if (turn == 0) { route }  else { [ (x, y), ... route] };
        /* let nxt: list((int, int)) = [ (x, y), ... routes ]; */
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
    
    let rec recRoutes = ((x, y), turn, current, routes: list(list((int, int)))) => {
      if (turn > limit) routes
      else if (limit - turn < Js.Math.abs_int(tx - x) || limit - turn < Js.Math.abs_int(ty - y)) routes
      else if (invalidPosition(x, y)) routes
      else if (isOutOfBounds(x, y, maxX, maxY)) routes
      else if (isInvalidTerrain(x, y, area)) routes
      else if (turn == 1 && isInvalidMove(x, y, area)) routes
      else if (routes |> Rationale.RList.any(p => List.length(current) > List.length(p))) routes
      else if (isGoal(x, y, tx, ty)) [ [(x, y), ...current], ... routes ]
      else if (turn != 0 && Rationale.RList.any(xy => {let (ox, oy) = xy; ox == x && oy == y}, current)) routes
      else {
        let history: list((int, int)) = if (turn == 0) { current }  else { [ (x, y), ... current ] };
      
        recRoutes((x, y + 1), turn + 1, history, 
          recRoutes((x, y - 1), turn + 1, history, 
          recRoutes((x - 1 , y), turn + 1, history, 
          recRoutes((x + 1, y), turn + 1, history, 
          recRoutes((x - 1, y + 1), turn + 1, history, 
          recRoutes((x + 1, y + 1), turn + 1, history,
          recRoutes((x - 1, y - 1), turn + 1, history,
          recRoutes((x + 1, y - 1), turn + 1, history, routes)))))))
        );
      }
    };

    recRoutes((x, y), 0, [], []);
  };
  
  type coord = { x: int, y: int};

  let findFastestRoutes = (~limit=4, ~incTerrain=true, area, (x, y), (tx, ty)) => {
    let countPenalties: list((int, int)) => float = locations => 
      if (incTerrain) {  
        locations
          |> List.map(loc => { let (x, y) = loc; area |> List.nth(_, y) |> List.nth(_, x) |>  Level.Tiles.placePenalty})
          |> List.fold_left((p1, p2) => p1 +. p2, 0.)
      } else locations |> List.length |> float_of_int;

    let routes = findRoutes(~limit=limit, area, (x, y), (tx, ty));
    
    if (List.length(routes) <= 1) routes
    else {
      let shortest = List.fold_left(
        (len, route) => if (countPenalties(route) > len) len else countPenalties(route),
        99., routes);

      List.filter(route => countPenalties(route) == shortest, routes);
    }
  };
};

module Navigation: Movement = {

  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) =>
    PathUtil.canNavigateTo(~limit=limit, area, (x, y), (tx, ty));

  let suggestMove = (~limit=4, ~incTerrain=true, area, (x, y), (tx, ty)) => {
    if (canNavigateTo(~limit=limit, area, (x, y), (tx, ty))) {
      let bestMoves = PathUtil.findFastestRoutes(~limit=limit, ~incTerrain=incTerrain, area, (x, y), (tx, ty));
      let (bx, by) = bestMoves |> List.hd |> List.rev |> List.hd;
      (bx - x, by - y);
    } else {
      (0, 0);
    }
  };
};