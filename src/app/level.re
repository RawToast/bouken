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
          if (xi == y) {
              xs |> 
               List.mapi((yi: int, place: place) =>
                 if (yi == x) newPlace else place);
          } else xs
        );
      { name: level.name, map: updateMap(level.map) };
  };

  let isPlayer = place => switch place.state {
      | EMPTY => false
      | ENEMY(_) => false
      | PLAYER(_) => true
      };

  let findPlayer = (level:level) => {
    let findPlayerRow = RList.find(y => isPlayer(y));
    let hasPlayer = (p:option(place)) => Option.isSome(p);

    level.map 
      |> RList.find(row => row |> findPlayerRow |> hasPlayer)
      |> Option.default([])
      |> RList.find(state => isPlayer(state))
      |> Option.bind(_, s => switch(s.state) {
        | EMPTY => None
        | ENEMY(_) => None
        | PLAYER(p) => Some(p)
      });
  };

  let canMoveTo = (x, y, map: list(list(place))) => map 
    |> RList.nth(x) 
    |> Option.bind(_, RList.nth(y))
    |> Result.ofOption(IMPOSSIBLE_MOVE)
    |> Result.bind(_, l => switch l.tile {
        | GROUND => success(l)
        | WATER => success(l)
        | WALL => error(IMPOSSIBLE_MOVE)
    });

  let getPlace = (x, y, map) => 
      map |> RList.nth(x) |> Option.bind(_, RList.nth(y));

  let movePlayer = (x: int, y: int, level: level) => {

    let update = (player, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == x) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == y) { 
              { ...place, state: PLAYER({ ...player, location: (x, y) }) }
            } else place);
        } else xs
      );
    };

    canMoveTo(x, y, level.map) 
      |> Result.bind(_, _r =>
        findPlayer(level) 
          |> Option.fmap(p => { name: level.name, map: update(p, level.map) }) 
          |> o => switch (o) {
          | None => error(INVALID_STATE)
          | Some(result) => success(result)
          })
  };
};