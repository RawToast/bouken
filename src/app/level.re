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

module Tiles = {
  let wallTile = {tile: WALL, state: EMPTY};
};

module Area = {
  let findPlayer = (area:area) => {
    let findPlayerRow = RList.find(y => isPlayer(y));
    let hasPlayer = (p:option(place)) => Option.isSome(p);

    area
      |> RList.find(row => row |> findPlayerRow |> hasPlayer)
      |> Option.default([])
      |> RList.find(state => isPlayer(state))
      |> Option.bind(_, s => switch(s.state) {
        | PLAYER(p) => Some(p)
        | _ => None
      });
  };

  let findEnemy: (string, area) => option(enemy) = (id, area) => {
    let isEnemyWithId = (id, place) => switch place.state {
      | ENEMY(e) => e.id === id
      | _ => false
      };
    let hasEnemy = (p:option(place)) => Option.isSome(p);

    let findEnemyRow = (id, places) => 
      places |> RList.find(y => isEnemyWithId(id, y));

    area
      |> RList.find(row => row |> findEnemyRow(id) |> hasEnemy)
      |> Option.default([])
      |> RList.find(p => isEnemyWithId(id, p))
      |> Option.bind(_, s => switch(s.state) {
        | EMPTY => None
        | ENEMY(e) => Some(e)
        | PLAYER(_) => None
      });
  };

  let canMoveTo = (x, y, map: area) => map 
    |> RList.nth(y) 
    |> Option.bind(_, RList.nth(x))
    |> Result.ofOption(IMPOSSIBLE_MOVE)
    |> Result.bind(_, l => switch l.tile {
        | GROUND => success(l)
        | WATER => success(l)
        | WALL => error(IMPOSSIBLE_MOVE)
    });

    let removeOccupant = (x, y, area) => {
      area |> 
        List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              { ...place, state: EMPTY }
            } else place);
        } else xs
      );
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

  let modifyTiles = (points: list((int, int)), newPlace: place, level: level) =>
    points |> List.fold_left((l, ((x:int, y: int))) => modifyTile(x, y, newPlace, l), level);

  let findPlayer = level => Area.findPlayer(level.map);

  let findEnemy = (id, level) => Area.findEnemy(id, level.map);

  let getPlace = (x, y, map) => 
      map |> RList.nth(y) |> Option.bind(_, RList.nth(x));

  let removeOccupant = (x, y, level) => { ...level, map: Area.removeOccupant(x, y, level.map) };

  let setPlayerLocation = (x: int, y: int, level: level) => {
    let update = (player, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              { ...place, state: PLAYER({ ...player, location: (x, y) }) }
            } else place);
        } else xs
      );
    };

    Area.canMoveTo(x, y, level.map) 
      |> Result.bind(_, _r =>
       findPlayer(level) 
          |> Option.fmap(p => { name: level.name, map: update(p, level.map) }) 
          |> o => switch (o) {
          | None => error(INVALID_STATE)
          | Some(result) => success(result)
          })
  };

  let movePlayer(x: int, y: int, level: level) = {
    let playerOpt = findPlayer(level);
    switch(playerOpt) {
    | Some(player) => {
      let (xl, yl) = player.location;
      let nx = x + xl;
      let ny = y + yl;

      level 
        |> setPlayerLocation(nx, ny)
        |> Result.fmap(removeOccupant(xl, yl))
    }
    | None => error(INVALID_STATE);
    };
  };


};