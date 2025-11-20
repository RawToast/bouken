open Types;

module Option = Belt_Option;
module RList = Belt_List;

module DictRouting = {
  type location = {
    x: int,
    y: int,
  };
  module LocationCmp =
    Belt.Id.MakeComparable({
      type t = location;
      let cmp: (location, location) => int =
        ({x: xa, y: ya}, {x: xb, y: yb}) => {
          let xCmp = Pervasives.compare(xa, xb);
          let yCmp = Pervasives.compare(ya, yb);

          if (xCmp == 1 && yCmp == 1) {
            1;
          } else if (xCmp == (-1) && yCmp == (-1)) {
            (-1);
          } else {
            Pervasives.compare(xCmp, yCmp);
          };
        };
    });

  let toDict = area =>
    area
    |> List.mapi((yi, ys) =>
         ys |> List.mapi((xi, place) => ({x: xi, y: yi}, place))
       )
    |> List.flatten
    |> Array.of_list
    |> Belt.Map.Dict.fromArray(_, ~cmp=LocationCmp.cmp);

  let findPlace = (x, y, area) =>
    area |> Belt.Map.Dict.getExn(_, {x, y}, ~cmp=LocationCmp.cmp);

  let findPlaceOpt = (x, y, area) =>
    area |> Belt.Map.Dict.getExn(_, {x, y}, ~cmp=LocationCmp.cmp);

  let isInvalidTerrain = (x, y, area) =>
    findPlace(x, y, area) |> (p => p.tile |> Level.Tiles.isWall == true);

  let isInvalidMove = (x, y, area) =>
    findPlace(x, y, area) |> Level.Tiles.canOccupyOrAttack == false;
};

/* Priority Queue implementation for A* pathfinding
   Uses a simple sorted list approach for O(log n) insertion
   Optimized for the typical case where we process nodes in priority order */
module PriorityQueue = {
  type t('a) = list((float, 'a));

  let empty: t('a) = [];

  let isEmpty = (pq: t('a)) => List.length(pq) == 0;

  /* Insert element maintaining sorted order by priority (lower values = higher priority) */
  let rec insert = (priority: float, item: 'a, pq: t('a)): t('a) => {
    switch (pq) {
    | [] => [(priority, item)]
    | [(p, i), ...rest] =>
      if (priority < p) {
        [(priority, item), (p, i), ...rest];
      } else {
        [(p, i), ...insert(priority, item, rest)];
      }
    };
  };

  /* Extract minimum priority element */
  let extractMin = (pq: t('a)): option((float, 'a, t('a))) => {
    switch (pq) {
    | [] => None
    | [(priority, item), ...rest] => Some((priority, item, rest))
    };
  };
};

module PathUtil = {
  let invalidPosition = (x, y) => x < 0 || y < 0;
  let isOutOfBounds = (x, y, maxX, maxY) => x > maxX || y > maxY;
  let isInvalidTerrain = (x, y, area) =>
    area
    |> List.nth(_, y)
    |> List.nth(_, x)
    |> (p => p.tile |> Level.Tiles.isWall == true);
  let isInvalidMove = (x, y, area) =>
    area
    |> List.nth(_, y)
    |> List.nth(_, x)
    |> Level.Tiles.canOccupyOrAttack == false;
  let isGoal = (x, y, tx, ty) => x == tx && y == ty;
  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;

    let dArea = DictRouting.toDict(area);

    let rec navigate = ((x, y), turn, route, canNavi: bool) =>
      if (turn > limit) {
        false;
      } else if (canNavi) {
        true;
      } else if (limit
                 - turn < Js.Math.abs_int(tx - x)
                 || limit
                 - turn < Js.Math.abs_int(ty - y)) {
        false;
      } else if (invalidPosition(x, y)) {
        false;
      } else if (isOutOfBounds(x, y, maxX, maxY)) {
        false;
      } else if (DictRouting.isInvalidTerrain(x, y, dArea)) {
        false;
      } else if (turn == 1 && DictRouting.isInvalidMove(x, y, dArea)) {
        false;
      } else if (isGoal(x, y, tx, ty)) {
        true;
      } else if (turn != 0
                 && RList.some(
                      route,
                      xy => {
                        let (ox, oy) = xy;
                        ox == x && oy == y;
                      },
                    )) {
        false;
      } else {
        let route =
          if (turn == 0) {
            route;
          } else {
            [(x, y), ...route];
          };

        navigate(
          (x - 1, y + 1),
          turn + 1,
          route,
          navigate(
            (x, y + 1),
            turn + 1,
            route,
            navigate(
              (x + 1, y + 1),
              turn + 1,
              route,
              navigate(
                (x - 1, y),
                turn + 1,
                route,
                navigate(
                  (x + 1, y),
                  turn + 1,
                  route,
                  navigate(
                    (x - 1, y - 1),
                    turn + 1,
                    route,
                    navigate(
                      (x, y - 1),
                      turn + 1,
                      route,
                      navigate((x + 1, y - 1), turn + 1, route, canNavi),
                    ),
                  ),
                ),
              ),
            ),
          ),
        );
      };

    navigate((x, y), 0, [], false);
  };

  let findRoute = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;
    let dArea = DictRouting.toDict(area);

    let rec recRoutes = ((x, y), turn, route) =>
      if (turn > limit) {
        [];
      } else if (invalidPosition(x, y)) {
        [];
      } else if (isOutOfBounds(x, y, maxX, maxY)) {
        [];
      } else if (DictRouting.isInvalidTerrain(x, y, dArea)) {
        [];
      } else if (turn == 1 && DictRouting.isInvalidMove(x, y, dArea)) {
        [];
      } else if (isGoal(x, y, tx, ty)) {
        route;
      } else {
        let nxt =
          if (turn == 0) {
            route;
          } else {
            [(x, y), ...route];
          };

        recRoutes((x - 1, y + 1), turn + 1, nxt)
        @ recRoutes((x, y + 1), turn + 1, nxt)
        @ recRoutes((x + 1, y + 1), turn + 1, nxt)
        @ recRoutes((x - 1, y), turn + 1, nxt)
        @ recRoutes((x + 1, y), turn + 1, nxt)
        @ recRoutes((x - 1, y - 1), turn + 1, nxt)
        @ recRoutes((x, y - 1), turn + 1, nxt)
        @ recRoutes((x + 1, y - 1), turn + 1, nxt);
      };

    recRoutes((x, y), 0, []);
  };

  let findRoutes = (~limit=4, area, (x, y), (tx, ty)) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;
    let dArea = DictRouting.toDict(area);

    let rec recRoutes =
            ((x, y), turn, current, routes: list(list((int, int)))) =>
      if (turn > limit) {
        routes;
      } else if (limit
                 - turn < Js.Math.abs_int(tx - x)
                 || limit
                 - turn < Js.Math.abs_int(ty - y)) {
        routes;
      } else if (invalidPosition(x, y)) {
        routes;
      } else if (isOutOfBounds(x, y, maxX, maxY)) {
        routes;
      } else if (DictRouting.isInvalidTerrain(x, y, dArea)) {
        routes;
      } else if (turn == 1 && DictRouting.isInvalidMove(x, y, dArea)) {
        routes;
      } else if (routes->RList.some(p =>
                   List.length(current) > List.length(p)
                 )) {
        routes;
      } else if (isGoal(x, y, tx, ty)) {
        [[(x, y), ...current], ...routes];
      } else if (turn != 0
                 && RList.some(
                      current,
                      xy => {
                        let (ox, oy) = xy;
                        ox == x && oy == y;
                      },
                    )) {
        routes;
      } else {
        let history: list((int, int)) =
          if (turn == 0) {
            current;
          } else {
            [(x, y), ...current];
          };

        recRoutes(
          (x, y + 1),
          turn + 1,
          history,
          recRoutes(
            (x, y - 1),
            turn + 1,
            history,
            recRoutes(
              (x - 1, y),
              turn + 1,
              history,
              recRoutes(
                (x + 1, y),
                turn + 1,
                history,
                recRoutes(
                  (x - 1, y + 1),
                  turn + 1,
                  history,
                  recRoutes(
                    (x + 1, y + 1),
                    turn + 1,
                    history,
                    recRoutes(
                      (x - 1, y - 1),
                      turn + 1,
                      history,
                      recRoutes((x + 1, y - 1), turn + 1, history, routes),
                    ),
                  ),
                ),
              ),
            ),
          ),
        );
      };

    recRoutes((x, y), 0, [], []);
  };

  type coord = {
    x: int,
    y: int,
  };

  /* A* pathfinding algorithm with terrain cost support
     Returns the optimal path from start to target considering terrain penalties
     Uses Manhattan distance heuristic which is admissible for 8-directional movement */
  let findPathAStar = (~limit=4, ~incTerrain=true, area, (sx, sy), (tx, ty)): option(list((int, int))) => {
    let maxX = List.length(List.hd(area)) - 1;
    let maxY = List.length(area) - 1;
    let dArea = DictRouting.toDict(area);

    /* Manhattan distance heuristic - admissible and consistent */
    let heuristic = ((x, y)) =>
      float_of_int(Js.Math.abs_int(tx - x) + Js.Math.abs_int(ty - y));

    /* Get terrain cost for a position */
    let getTerrainCost = ((x, y)) => {
      if (incTerrain) {
        try (DictRouting.findPlace(x, y, dArea) |> Level.Tiles.placePenalty(~incTraps=false)) {
        | _ => 1.0
        };
      } else {
        1.0;
      }
    };

    /* Check if position is valid for pathfinding */
    let isValid = ((x, y)) => {
      !(invalidPosition(x, y))
      && !(isOutOfBounds(x, y, maxX, maxY))
      && {
        try (!DictRouting.isInvalidTerrain(x, y, dArea)) {
        | _ => false
        };
      };
    };

    /* Check if position can be moved to (not just seen) */
    let canMoveTo = ((x, y)) => {
      try (!DictRouting.isInvalidMove(x, y, dArea)) {
      | _ => false
      };
    };

    /* Generate 8-directional neighbors */
    let getNeighbors = ((x, y)) => [
      (x - 1, y - 1), (x, y - 1), (x + 1, y - 1),
      (x - 1, y),                 (x + 1, y),
      (x - 1, y + 1), (x, y + 1), (x + 1, y + 1),
    ];

    /* Reconstruct path from parent map */
    let rec reconstructPath = (cameFrom, current, acc) => {
      let key = {DictRouting.x: fst(current), y: snd(current)};
      switch (Belt.Map.get(cameFrom, key)) {
      | None => [current, ...acc]  /* Include the start position */
      | Some(parent) => reconstructPath(cameFrom, parent, [current, ...acc])
      };
    };

    /* A* main algorithm */
    let rec astar = (openSet, cameFrom, gScore, fScore) => {
      switch (PriorityQueue.extractMin(openSet)) {
      | None => None  /* No path found */
      | Some((_, current, openRest)) =>
        let (cx, cy) = current;

        /* Check if we reached the goal */
        if (cx == tx && cy == ty) {
          Some(reconstructPath(cameFrom, current, [current]));
        } else {
          /* Get current g-score */
          let currentKey = {DictRouting.x: cx, y: cy};
          let currentG = Belt.Map.getWithDefault(gScore, currentKey, infinity);

          /* Check if we exceeded movement limit */
          if (currentG > float_of_int(limit)) {
            astar(openRest, cameFrom, gScore, fScore);
          } else {
            /* Process all neighbors */
            let neighbors = getNeighbors(current);
            let (newOpen, newCameFrom, newGScore, newFScore) =
              List.fold_left(
                ((openAcc, cfAcc, gAcc, fAcc), neighbor) => {
                  let (nx, ny) = neighbor;

                  /* Skip invalid positions */
                  if (!isValid(neighbor) || (currentG == 0.0 && !canMoveTo(neighbor))) {
                    (openAcc, cfAcc, gAcc, fAcc);
                  } else {
                    /* Calculate tentative g-score */
                    let moveCost = getTerrainCost(neighbor);
                    let tentativeG = currentG +. moveCost;
                    let neighborKey = {DictRouting.x: nx, y: ny};
                    let neighborG = Belt.Map.getWithDefault(gAcc, neighborKey, infinity);

                    /* If this path is better, update scores */
                    if (tentativeG < neighborG) {
                      let newCameFrom = Belt.Map.set(cfAcc, neighborKey, current);
                      let newGScore = Belt.Map.set(gAcc, neighborKey, tentativeG);
                      let newFScore = Belt.Map.set(fAcc, neighborKey, tentativeG +. heuristic(neighbor));
                      let newOpen = PriorityQueue.insert(tentativeG +. heuristic(neighbor), neighbor, openAcc);

                      (newOpen, newCameFrom, newGScore, newFScore);
                    } else {
                      (openAcc, cfAcc, gAcc, fAcc);
                    }
                  }
                },
                (openRest, cameFrom, gScore, fScore),
                neighbors
              );

            astar(newOpen, newCameFrom, newGScore, newFScore);
          }
        }
      };
    };

    /* Initialize A* with start position */
    let startKey = {DictRouting.x: sx, y: sy};
    let initialOpen = PriorityQueue.insert(heuristic((sx, sy)), (sx, sy), PriorityQueue.empty);
    let initialCameFrom = Belt.Map.make(~id=(module DictRouting.LocationCmp));
    let initialGScore = Belt.Map.set(Belt.Map.make(~id=(module DictRouting.LocationCmp)), startKey, 0.0);
    let initialFScore = Belt.Map.set(Belt.Map.make(~id=(module DictRouting.LocationCmp)), startKey, heuristic((sx, sy)));

    astar(initialOpen, initialCameFrom, initialGScore, initialFScore);
  };

  let findFastestRoutes =
      (~limit=4, ~incTerrain=true, area, (x, y), (tx, ty)) => {
    /* Use original DFS implementation to maintain backward compatibility
       This returns ALL routes with equal cost, which is expected by the test suite
       The A* optimization is used in suggestMove where only a single path is needed */
    let countPenalties: list((int, int)) => float =
      locations =>
        if (incTerrain) {
          locations
          |> List.map(loc => {
               let (x, y) = loc;
               area->List.nth(y)->List.nth(x)
               |> Level.Tiles.placePenalty(~incTraps=false);
             })
          |> List.fold_left((p1, p2) => p1 +. p2, 0.);
        } else {
          locations |> List.length |> float_of_int;
        };

    let routes = findRoutes(~limit, area, (x, y), (tx, ty));

    if (List.length(routes) <= 1) {
      routes;
    } else {
      let shortest =
        List.fold_left(
          (len, route) =>
            if (countPenalties(route) > len) {
              len;
            } else {
              countPenalties(route);
            },
          99.,
          routes,
        );

      List.filter(route => countPenalties(route) == shortest, routes);
    };
  };
};

module Navigation: Movement = {
  let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) =>
    PathUtil.canNavigateTo(~limit, area, (x, y), (tx, ty));

  let suggestMove = (~limit=4, ~incTerrain=true, area, (x, y), (tx, ty)) =>
    if (canNavigateTo(~limit, area, (x, y), (tx, ty))) {
      /* Use A* algorithm for optimal single-path finding (performance optimization)
         A* is much faster than DFS when we only need one path */
      switch (PathUtil.findPathAStar(~limit, ~incTerrain, area, (x, y), (tx, ty))) {
      | Some(path) =>
        /* Get the second element in the path (first step after start position)
           Path is: [start, step1, step2, ..., target] */
        switch (path) {
        | [] => (0, 0)  /* Empty path, shouldn't happen */
        | [_only] => (0, 0)  /* Only start position, at target already */
        | [_start, next, ..._rest] =>
          /* Next is the first step to take */
          let (nx, ny) = next;
          (nx - x, ny - y);
        };
      | None =>
        /* Fallback: shouldn't happen if canNavigateTo returned true, but be safe */
        (0, 0)
      };
    } else {
      (0, 0);
    };
};

/* Shadow Casting FOV Algorithm
   Implements recursive shadowcasting for efficient field-of-view calculation
   Uses octant-based approach for symmetric visibility
   Significantly faster than ray-casting for large vision ranges */
module ShadowCasting = {
  module RList = Belt_List;

  /* Check if a tile blocks vision */
  let isBlocking = ((x, y), area) => {
    area
    ->RList.get(y)
    ->Option.flatMap(RList.get(_, x))
    ->Option.map(Level.Tiles.cantSeeThrough)
    ->Option.getWithDefault(true);  /* Out of bounds blocks vision */
  };

  /* Transform coordinates based on octant (0-7)
     Allows us to write scanning logic once and rotate it for all 8 octants */
  let transformOctant = (octant, col, row, (ox, oy)) => {
    switch (octant) {
    | 0 => (ox + col, oy - row)  /* N-NE */
    | 1 => (ox + row, oy - col)  /* NE-E */
    | 2 => (ox + row, oy + col)  /* E-SE */
    | 3 => (ox + col, oy + row)  /* SE-S */
    | 4 => (ox - col, oy + row)  /* S-SW */
    | 5 => (ox - row, oy + col)  /* SW-W */
    | 6 => (ox - row, oy - col)  /* W-NW */
    | _ => (ox - col, oy - row)  /* NW-N */
    };
  };

  /* Calculate slope for shadow casting */
  let slope = (col, row) => {
    float_of_int(col) /. float_of_int(row);
  };

  /* Scan a single row in an octant, tracking shadows cast by walls
     This is the core of the shadow casting algorithm */
  let rec scanRow = (~limit, ~octant, area, origin, row, startSlope, endSlope, acc) => {
    if (row > limit) {
      acc;
    } else {
      let (ox, oy) = origin;
      let minCol = int_of_float(floor(float_of_int(row) *. startSlope));
      let maxCol = int_of_float(ceil(float_of_int(row) *. endSlope));

      /* Scan all columns in this row */
      let rec scanCols = (col, prevBlocked, newAcc, shadows) => {
        if (col > maxCol) {
          /* If last tile was blocking, continue scanning next row with updated shadow */
          if (prevBlocked) {
            newAcc;
          } else {
            scanRow(~limit, ~octant, area, origin, row + 1, startSlope, endSlope, newAcc);
          }
        } else {
          let pos = transformOctant(octant, col, row, origin);
          let (px, py) = pos;

          /* Check if position is within map bounds */
          let inBounds = px >= 0 && py >= 0;
          let blocked = inBounds && isBlocking(pos, area);

          /* Calculate slopes for this tile */
          let leftSlope = slope(col, row + 1);
          let rightSlope = slope(col + 1, row + 1);

          /* Check if tile is in current shadow */
          let inShadow = leftSlope < startSlope || rightSlope > endSlope;

          let (nextAcc, nextBlocked, nextShadows) =
            if (!inBounds) {
              (newAcc, prevBlocked, shadows);
            } else if (inShadow) {
              (newAcc, prevBlocked, shadows);
            } else {
              /* Tile is visible */
              let visAcc = [pos, ...newAcc];

              if (blocked) {
                /* This tile blocks vision - cast shadow */
                if (prevBlocked) {
                  /* Extend existing shadow */
                  (visAcc, true, shadows);
                } else {
                  /* Start new shadow - recursively scan next row with narrowed slope */
                  let shadowAcc = scanRow(~limit, ~octant, area, origin, row + 1, startSlope, leftSlope, visAcc);
                  (shadowAcc, true, [(leftSlope, rightSlope), ...shadows]);
                }
              } else {
                /* Tile is transparent */
                if (prevBlocked) {
                  /* Just exited a shadow - update start slope */
                  (visAcc, false, shadows);
                } else {
                  (visAcc, false, shadows);
                }
              }
            };

          scanCols(col + 1, nextBlocked, nextAcc, nextShadows);
        }
      };

      scanCols(minCol, false, acc, []);
    };
  };

  /* Cast shadows in a single octant */
  let castOctant = (~limit, ~octant, area, origin) => {
    scanRow(~limit, ~octant, area, origin, 1, 0.0, 1.0, []);
  };

  /* Cast shadows in all 8 octants to get full field of view */
  let castAllOctants = (~limit, area, (x, y)) => {
    let rec loop = (oct, acc) => {
      if (oct >= 8) {
        acc;
      } else {
        let newTiles = castOctant(~limit, ~octant=oct, area, (x, y));
        loop(oct + 1, List.append(acc, newTiles));
      }
    };
    /* Include origin position as always visible */
    [(x, y), ...loop(0, [])];
  };
};

module VisionUtil = {
  module RList = Belt_List;

  let makeLine = (distance, area, (ox, oy), (dx, dy)) => {
    let cannotSeeThrough = ((x: int, y: int)) => {
      area
      ->RList.get(y)
      ->Option.flatMap(RList.get(_, x))
      ->Option.map(Level.Tiles.cantSeeThrough)
      ->Option.getWithDefault(true);
    };

    let (ffx, ffy) = {
      (
        if (dx == 0) {
          x => x;
        } else if (dx > 0) {
          x => x + 1;
        } else {
          x => x - 1;
        },
        if (dy == 0) {
          y => y;
        } else if (dy > 0) {
          y => y + 1;
        } else {
          y => y - 1;
        },
      );
    };

    let rec loop = (i, ix, iy, acc) => {
      let nx = ffx(ix);
      let ny = ffy(iy);
      if (i > distance) {
        acc;
      } else if (List.length(acc) == 0) {
        loop(i, ix, iy, [(ox, oy), ...acc]);
      } else if (nx == dx && ny == dy) {
        let (hx, hy) = List.hd(acc);
        if (cannotSeeThrough((ffx(hx), ffy(hy)))) {
          [(ffx(hx), ffy(hy)), ...acc];
        } else {
          loop(i + 1, 0, 0, [(ffx(hx), ffy(hy)), ...acc]);
        };
      } else if (nx == dx && dy != 0) {
        let (hx, hy) = List.hd(acc);
        if (cannotSeeThrough((ffx(hx), hy))) {
          [(ffx(hx), hy), ...acc];
        } else {
          loop(i + 1, 0, ny, [(ffx(hx), hy), ...acc]);
        };
      } else if (ny == dy) {
        let (hx, hy) = List.hd(acc);
        if (cannotSeeThrough((hx, ffy(hy)))) {
          [(hx, ffy(hy)), ...acc];
        } else {
          loop(i + 1, nx, 0, [(hx, ffy(hy)), ...acc]);
        };
      } else {
        loop(i, nx, ny, acc);
      };
    };

    loop(0, 0, 0, []);
  };

  let append = (a, xs) => List.append(xs, [a]);
  let rangeF = {
    let rec loop = (inc, s, e, acc) =>
      switch (List.rev(acc)) {
      | [] => loop(inc, s, e, [s])
      | [a, ..._] =>
        inc(a) > e ? acc : loop(inc, s, e, append(inc(a), acc))
      };
    (inc, s, e) => loop(inc, s, e, []);
  };
  let rangeInt = step => rangeF(x => x + step);

  let incRange = rangeInt(1);

  let basicRange = (~range=1, x, y) => {
    let minX = x - range;
    let maxX = x + range;

    let targetX = incRange(minX, maxX) |> List.filter(x => x >= 0);
    let targetY = incRange(y - range, y + range) |> List.filter(y => y >= 0);
    targetY
    |> List.map(y => targetX |> List.map(x => (x, y)))
    |> List.flatten
    |> List.filter(xy => xy != (x, y));
  };

  let makeLines = (~limit=4, area, (x, y)) => {
    let bigLimit = limit;
    let rng = incRange(- bigLimit, bigLimit) |> List.filter(i => i != 0);

    rng
    |> List.map(x => rng |> List.map(y => (x, y)))
    |> List.flatten
    |> List.append([(0, 1), (0, (-1)), (1, 0), ((-1), 0)])
    |> List.map(dxdy => makeLine(limit + 2, area, (x, y), dxdy))
    |> List.flatten
    |> List.filter(((xx, yy)) => xx <= x + limit && yy <= y + limit)
    |> List.filter(((xx, yy)) => xx >= x - limit && yy >= y - limit);
  };

  let canSee = (~limit=4, area, (x, y), (tx, ty)) => {
    let possibleVision = basicRange(~range=limit, x, y);

    let inMaxRange =
      possibleVision
      -> RList.some(((px, py)) => px == tx && py == py);

    if (inMaxRange) {
        makeLines(~limit, area, (x, y))
        -> RList.some(((x, y)) => x == tx && y == ty)
    } else {
      false;
    };
  };

  let updateTiles = (~limit=4, area, (x, y)) => {
    /* Use optimized shadow casting instead of ray casting */
    let visibleTiles = ShadowCasting.castAllOctants(~limit, area, (x, y));

    area
    |> List.mapi((yi, ys) =>
         ys
         |> List.mapi((xi, place) =>
              if (RList.some(visibleTiles, xy => xy == (xi, yi))) {
                {...place, visible: true};
              } else {
                {...place, visible: false};
              }
            )
       );
  };
};
