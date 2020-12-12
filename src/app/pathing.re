open Types;

module DictRouting = {
  type location = {
    x: int,
    y: int
  };
  module LocationCmp = Belt.Id.MakeComparable({
    type t = location;
    let cmp: (location, location) => int = 
      ({x: xa, y: ya}, {x: xb, y: yb}) => {
        let xCmp = Pervasives.compare(xa, xb);
        let yCmp = Pervasives.compare(ya, yb);

        if (xCmp == 1 && yCmp == 1) 1
        else if (xCmp == -1 && yCmp == -1) (-1)
        else Pervasives.compare(xCmp, yCmp)
      }
  });

  let toDict = area => area |> List.mapi((yi, ys) => ys |> List.mapi((xi, place) => ({x: xi, y: yi}, place))) 
    |> List.flatten 
    |> Array.of_list
    |> Belt.Map.Dict.fromArray(_, ~cmp=LocationCmp.cmp);

  let findPlace = (x, y, area) => 
    (area |> Belt.Map.Dict.getExn(_, {x: x, y: y}, ~cmp=LocationCmp.cmp));

  let findPlaceOpt = (x, y, area) => 
    (area |> Belt.Map.Dict.getExn(_, {x: x, y: y}, ~cmp=LocationCmp.cmp));

  let isInvalidTerrain = (x, y, area) => findPlace(x, y, area)
    |> p => (p.tile |> Level.Tiles.isWall) == true;

  let isInvalidMove = (x, y, area) => findPlace(x, y, area)
      |> Level.Tiles.canOccupyOrAttack == false;
};

module PathUtil = {

  let invalidPosition = (x, y) => (x < 0 || y < 0);
  let isOutOfBounds = (x, y, maxX, maxY) => (x > maxX || y > maxY);
  let isInvalidTerrain = (x, y, area) => (area |> List.nth(_, y) |> List.nth(_, x) |> p => p.tile |> Level.Tiles.isWall == true);
  let isInvalidMove = (x, y, area) => (area |> List.nth(_, y) |> List.nth(_, x) |> Level.Tiles.canOccupyOrAttack == false);
  let isGoal = (x, y, tx, ty) => (x == tx && y == ty);
  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let dArea = DictRouting.toDict(area);

    let rec navigate = ((x, y), turn, route, canNavi: bool) => {
      if (turn > limit) false
      else if (canNavi) true
      else if (limit - turn < Js.Math.abs_int(tx - x) || limit - turn < Js.Math.abs_int(ty - y)) false
      else if (invalidPosition(x, y)) false
      else if (isOutOfBounds(x, y, maxX, maxY)) false
      else if (DictRouting.isInvalidTerrain(x, y, dArea)) false
      else if (turn == 1 && DictRouting.isInvalidMove(x, y, dArea)) false
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
    let dArea = DictRouting.toDict(area);

    let rec recRoutes = ((x, y), turn, route) => {
      if (turn > limit) []
      else if (invalidPosition(x, y)) []
      else if (isOutOfBounds(x, y, maxX, maxY)) []
      else if (DictRouting.isInvalidTerrain(x, y, dArea)) []
      else if (turn == 1 &&  DictRouting.isInvalidMove(x, y, dArea)) []
      else if (isGoal(x, y, tx, ty)) route
      else {
        let nxt = if (turn == 0) { route }  else { [ (x, y), ... route] };

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
    let dArea = DictRouting.toDict(area);

    let rec recRoutes = ((x, y), turn, current, routes: list(list((int, int)))) => {
      if (turn > limit) routes
      else if (limit - turn < Js.Math.abs_int(tx - x) || limit - turn < Js.Math.abs_int(ty - y)) routes
      else if (invalidPosition(x, y)) routes
      else if (isOutOfBounds(x, y, maxX, maxY)) routes
      else if (DictRouting.isInvalidTerrain(x, y, dArea)) routes
      else if (turn == 1 && DictRouting.isInvalidMove(x, y, dArea)) routes
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
          |> List.map(loc => { let (x, y) = loc; 
            area -> List.nth(y) -> List.nth(x) |>  Level.Tiles.placePenalty(~incTraps=false)})
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

module VisionUtil = {
  
  let makeLine = (distance, area, (ox, oy), (dx, dy)) => {
    open Rationale.Option;

    let cannotSeeThrough = ((x, y)) => 
      area 
        |> Rationale.RList.nth(y) 
        >>= Rationale.RList.nth(x)
        <$> (Level.Tiles.cantSeeThrough)
        |> Rationale.Option.default(true); 

    let (ffx, ffy) = {
      ( if (dx == 0) { x => x }
        else if (dx > 0) { x => x + 1 }
        else { x => x - 1 }
      ,
      if (dy == 0) { y => y }
      else if (dy > 0) { y => y + 1 }
      else { y => y - 1 }
      )
    }
    
    let rec loop = (i, ix, iy, acc) => {
      let nx = ffx(ix);
      let ny = ffy(iy);
      if (i > distance) acc
      else if (List.length(acc) == 0) loop(i, ix, iy, [ (ox, oy), ... acc])
      else if (nx == dx && ny == dy) {
        let (hx, hy) = List.hd(acc); 
        if (cannotSeeThrough((ffx(hx), ffy(hy)))) [ (ffx(hx), ffy(hy)), ... acc]
        else loop(i + 1, 0, 0, [ (ffx(hx), ffy(hy)), ... acc]) }
      else if (nx == dx && dy != 0) { 
        let (hx, hy) = List.hd(acc); 
        if (cannotSeeThrough((ffx(hx), hy))) [ (ffx(hx), hy ), ... acc]
        else loop(i + 1, 0, ny, [ (ffx(hx), hy ), ... acc]) }
      else if (ny == dy) { 
        let (hx, hy) = List.hd(acc); 
        if (cannotSeeThrough(( hx, ffy(hy)))) [ (hx, ffy(hy)), ... acc]
        else loop(i + 1, nx, 0, [ (hx, ffy(hy)), ... acc]) }
      else loop(i, nx, ny, acc);
    };

    loop(0, 0, 0, []);
  };

  let incRange = Rationale.RList.rangeInt(1);

  let basicRange = (~range=1, x, y) => {
    let minX = x - range;
    let maxX = x + range;

    let targetX = incRange(minX, maxX) |> List.filter(x => x >= 0);
    let targetY = incRange((y-range), (y+range)) |> List.filter(y => y >= 0);
    targetY |> List.map(y => (targetX |> List.map(x => (x, y)))) |> List.flatten |> List.filter(xy => xy != (x, y));
  };

  let makeLines = (~limit=4, area, (x, y)) => {    
    let bigLimit = limit;  
    let rng = incRange(-bigLimit, bigLimit) |> List.filter(i => i != 0);

    rng
      |> List.map(x => rng |> List.map(y => ((x, y)))) 
      |> List.flatten 
      |> List.append([ (0, 1), (0, -1), (1, 0), (-1, 0) ])
      |> List.map(dxdy => makeLine(limit + 2, area, (x, y), dxdy))
      |> List.flatten 
      |> List.filter(((xx, yy)) => (xx <= x + limit) && (yy <= y + limit) )
      |> List.filter(((xx, yy)) => (xx >= x - limit) && (yy >= y - limit) );
  }

  let canSee = (~limit=4, area, (x, y), (tx, ty)) => {
    let possibleVision = basicRange(~range=limit, x, y);

    let inMaxRange = 
      possibleVision |>
        Rationale.RList.any(((px, py)) => px == tx && py == py);
  
    if (inMaxRange) {
      open Rationale.RList;

      makeLines(~limit=limit, area, (x, y))
       |> any(((x, y)) => x == tx && y == ty);
    } else false;
  };

  let updateTiles = (~limit=4, area, (x, y)) => {
    let lines = makeLines(~limit=limit, area, (x, y));

    area |> List.mapi((yi, ys) => ys |> List.mapi((xi, place) => 
        if (Rationale.RList.any((xy) => xy == (xi, yi), lines)) { ... place, visible: true } 
        else { ... place, visible: false }));
  }
};
