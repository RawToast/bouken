open Types;

open Rationale;

module LevelBuilder = {
  let blankPlace = {tile: GROUND, state: EMPTY};
  let makeBlankWorld = (name: string) => {
    let emptyMap =
      RList.repeat(blankPlace, 15) |> List.map(i => RList.repeat(i, 15));
    {name, map: emptyMap};
  };
};

module Level = {
  let modifyTile = (x: int, y: int, newPlace: place, level: level) => {
      let updateMap =
        List.mapi((xi: int, xs: list(place)) =>
          if (xi == x) {
              xs |> 
               List.mapi((yi: int, place: place) =>
                 if (yi == y) newPlace else place);
          } else xs
        );
      { name: level.name, map: updateMap(level.map) };
  };
};