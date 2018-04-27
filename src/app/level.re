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

  let isPlayer = state => switch state {
    | EMPTY => false
    | ENEMY(_) => false
    | PLAYER(_) => true
    };

  let findPlayer = (level:level) => {
    let findPlayerRow = RList.find(y => isPlayer(y.state));
    let hasPlayer = (p:option(place)) => Option.isSome(p);

    level.map 
    |> RList.find(row => row |> findPlayerRow |> hasPlayer)
    |> Option.default([])
    |> List.map(p => p.state)
    |> RList.find(state => isPlayer(state))
    |> Option.bind(_, s => switch(s) {
      | EMPTY => None
      | ENEMY(_) => None
      | PLAYER(p) => Some(p)
    });
  };

  let movePlayer = (x: int, y: int, level: level) => {

    let update = (player) =>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == x) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == y) { 
              { tile: place.tile, state: PLAYER({ ...player, location: (x, y) }) }
            } else place);
        } else xs
      );

      findPlayer(level) 
        |> Option.fmap(p => { name: level.name, map: update(p, level.map) });
  };
};