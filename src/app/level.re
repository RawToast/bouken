open Types;
open Rationale;

module LevelBuilder = {
  let blankPlace = {tile: GROUND, state: Empty};
  let makeBlankLevel = (name: string) => {
    let emptyMap =
      RList.repeat(blankPlace, 15) |> List.map(i => RList.repeat(i, 15));
    {name, map: emptyMap};
  };
  
  let makeWaterLevel = (name: string) => {
    let emptyMap =
      RList.repeat({tile: WATER, state: Empty}, 15) |> List.map(i => RList.repeat(i, 15));
    {name, map: emptyMap};
  };

  let makeLevel = (name, sizeX, sizeY, defaultTile) => {
    let emptyMap =
      RList.repeat({tile: defaultTile, state: Empty}, sizeY) |> List.map(i => RList.repeat(i, sizeX));
    {name, map: emptyMap};
  };
};

module Tiles = {
  let groundTile = {tile: GROUND, state: Empty};
  let wallTile = {tile: WALL, state: Empty};
  let waterTile = {tile: WATER, state: Empty};

  let isGround = t => switch t {
    | GROUND => true
    | _ => false
    };

  let isWall = t => switch t {
    | WALL => true
    | _ => false
    };

  let isEnemy = t => switch t.state {
    | Enemy(_) => true
    | _ => false
    };

  let isObject = t => switch t.state {
    | Object(_) => true
    | _ => false
    };

  let isStairs = t => switch t.tile {
    | STAIRS(_) => true
    | _ => false
    };

  let isEmpty = t => switch t.state {
    | Empty => true
    | _ => false
    };
  
  let isExit = t => switch t {
    | EXIT(_) => true
    | _ => false
    };

  let tilePenalty = t => switch t {
    | WATER => 1.5
    | _ => 1.
    };

  let statePenalty = t => switch t {
    | Enemy(_) => 3.
    | _ => 1.
    };

  let placePenalty = t => tilePenalty(t.tile) +. statePenalty(t.state);

  let canOccupy = p => 
    if (isEnemy(p)) false
    else if(isWall(p.tile)) false
    else isEmpty(p);

  let canOccupyOrAttack = p => 
    if (isEnemy(p)) false
    else if(isWall(p.tile)) false
    else true;
};

module Area: Places = {

  let findStairs = (id, area: list(list(place))) => {

    let findStairsRow = id => RList.find(y => switch(y.tile) {
    | STAIRS(link) => link.id == id
    | _ => false
    });

    area
     |> RList.find(row => row |> findStairsRow(id) |> Option.isSome)
     |> Option.default([])
     |> RList.find(place => switch(place.tile) {
      | STAIRS(link) => link.id == id
      | _ => false 
     })
  };

  let locationOfStairs = (id, area: list(list(place))) => {
    let findStairsRow = id => RList.findIndex(y => switch(y.tile) {
    | STAIRS(link) => link.id == id
    | _ => false
    });

    let idx = area |> RList.findIndex(row => row |> findStairsRow(id) |> Option.isSome);

    Option.bind(idx, y => {
      area 
        |> List.nth(_, y) 
        |> findStairsRow(id)
        |> Option.fmap(x => (x, y));
    });
  };

  let getPlace = (x, y, area) => area |> RList.nth(y) |> Option.bind(_, RList.nth(x));

  let findPlayer = (area) => {
    open Rationale.Option;

    let findPlayerRow = RList.find(y => isPlayer(y));
    let hasPlayer = (p:option(place)) => Option.isSome(p);

    area
      |> RList.find(row => row |> findPlayerRow |> hasPlayer)
      |> Option.default([])
      |> RList.find(state => isPlayer(state))
      >>= place => switch place.state {
        | Player(p) => Some(p)
        | _ => None
      };
  };

  let findEnemy: (string, area) => option(enemy) = (id, area) => {
    let isEnemyWithId = (id, place) => switch place.state {
      | Enemy(e) => e.id === id
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
        | Empty => None
        | Enemy(e) => Some(e)
        | Player(_) => None
        | Object(_) => None
      });
  };

  let isEmpty = (p:place) => switch p.state {
    | Empty => true
    | _ => false
  };

  let canMoveTo = (~overwrite=true, x, y, map: area) => {
    open Rationale.Option;
    
    map |> RList.nth(y) >>= RList.nth(x)
        |> Result.ofOption(ImpossibleMove)
        |> Result.bind(_, l => switch l.tile {
            | GROUND => if (isEmpty(l) || overwrite) success(l) else error(ImpossibleMove)
            | WATER => if (isEmpty(l) || overwrite) success(l) else error(ImpossibleMove)
            | WALL => error(ImpossibleMove)
            | STAIRS(_) => if (isEmpty(l) || overwrite) success(l) else error(ImpossibleMove)
            | EXIT(_) => if (isEmpty(l) || overwrite) success(l) else error(ImpossibleMove)
        });
  };
    
  let removeOccupant = (x, y, area) => {
      area |> 
        List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              { ...place, state: Empty }
            } else place);
        } else xs
      );
  };

  let setPlayerLocation = (x: int, y: int, cost: float, area: area) => {
    let update = (playerFunc, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              let np:player = playerFunc(place.tile);
              { ...place, state: Player({ ...np, location: (x, y) }) }
            } else place);
        } else xs
      );
    };

    canMoveTo(x, y, area) 
      |> Result.bind(_, _r =>
        findPlayer(area) 
        |> Option.fmap((p: player) => (tile => 
          {... p, stats: { ... p.stats, position: p.stats.position -. (Tiles.tilePenalty(tile) *. cost)}}))
        |> Option.fmap(p => update(p, area) ) 
        |> o => switch (o) {
          | None => error(InvalidState)
          | Some(result) => success(result)
      })
  };

  let setPlayerAt = (x: int, y: int, player: player, cost: float, area: area) => {
    let update = (tileToPlayer, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) {
              let updatedPlayer = tileToPlayer(place.tile);
              { ...place, state: Player({ ...updatedPlayer, location: (x, y) }) }
            } else place);
        } else xs
      );
    };

    canMoveTo(x, y, area) 
      |> Result.fmap(_ => {
        let updatedPlayer = tile => { 
          ... player, 
          stats: { 
            ... player.stats, 
            position: player.stats.position  -. (cost *. Tiles.tilePenalty(tile))
          }
        };
        let updatedArea = update(updatedPlayer, area);
        updatedArea;
      });
  };

  let setEnemyAt = (x: int, y: int, enemy: enemy, cost: float, area: area) => {
    let update = (e, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              { ...place, state: Enemy(e(place.tile)) }
            } else place);
        } else xs
      );
    };

    canMoveTo(x, y, area) 
      |> Result.fmap(_ => {
        let updatedEnemy = tile => 
          { ... enemy, 
            stats: { ... enemy.stats, position: (enemy.stats.position -. (cost *. Tiles.tilePenalty(tile)))}
          };
        let updatedArea = update(updatedEnemy, area);
        updatedArea;
      });
  };
  
  let movePlayer(x: int, y: int, cost:float, area: area) = {
    open Rationale.Result;
    let playerOpt = findPlayer(area);
    switch(playerOpt) {
      | Some(player) => {
          let (xl, yl) = player.location;
          let nx = x + xl;
          let ny = y + yl;

          let newPlayer = { ... player, location: (nx, ny)};

          if (x == 0 && y == 0) { /* Wait logic */
            area |> setPlayerLocation(nx, ny, cost) |> Result.fmap(a => { player: newPlayer, area: a})
          } else {
            area |> canMoveTo(~overwrite=false, nx, ny)
              >>= _ => setPlayerLocation(nx, ny, cost, area)
              |> Result.fmap(removeOccupant(xl, yl))
              |> Result.fmap(a => { player: newPlayer, area: a})
          };
      }
      | None => error(InvalidState);
    };
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

  let getPlace = (x, y, map) => map |> RList.nth(y) |> Option.bind(_, RList.nth(x));

  let removeOccupant = (x, y, level) => { ...level, map: Area.removeOccupant(x, y, level.map) };

  let setPlayerLocation = (x: int, y: int, level: level) => {
    let update = (player, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              { ...place, state: Player({ ...player, location: (x, y) }) }
            } else place);
        } else xs
      );
    };

    Area.canMoveTo(x, y, level.map) 
      |> Result.bind(_, _r =>
       findPlayer(level) 
          |> Option.fmap(p => { name: level.name, map: update(p, level.map) }) 
          |> o => switch (o) {
          | None => error(InvalidState)
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
      | None => error(InvalidState);
    };
  };
};
