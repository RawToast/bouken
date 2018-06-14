open Types;
let canNavigateTo = (~limit=4, area, (x, y), (tx, ty)) => {
  let rec navigate = ((x, y), turn) => {
    if (turn > limit) false
    else if(x < 0 && y < 0) false
    else if(x == tx && y == ty) true
    else navigate((x - 1, y + 1), turn + 1) || navigate((x, y + 1), turn + 1) || navigate((x + 1, y + 1), turn + 1) 
      || navigate((x - 1 , y), turn + 1) || navigate((x + 1, y), turn + 1)
      || navigate((x - 1, y - 1), turn + 1) || navigate((x, y - 1), turn + 1) || navigate((x + 1, y - 1), turn + 1)
  };

  navigate((x, y), 0);
};