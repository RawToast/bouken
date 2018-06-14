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

let findRoutes = (~limit=4, area, (x, y), (tx, ty)) => {
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