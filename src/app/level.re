open Types;
open Rationale;

module LevelBuilder = {
  let blankPlace = {tile: GROUND, state: Empty, tileEffect: NoEff, visible: false };
  let makeBlankLevel = (name: string) => {
    let emptyMap =
      RList.repeat(blankPlace, 15) |> List.map(i => RList.repeat(i, 15));
    {name, map: emptyMap};
  };
  
  let makeWaterLevel = (name: string) => {
    let emptyMap =
      RList.repeat({tile: WATER, state: Empty, tileEffect: NoEff, visible: false }, 15) |> List.map(i => RList.repeat(i, 15));
    {name, map: emptyMap};
  };

  let makeLevel = (name, sizeX, sizeY, defaultTile) => {
    let emptyMap =
      RList.repeat({tile: defaultTile, state: Empty, tileEffect: NoEff, visible: false }, sizeY) |> List.map(i => RList.repeat(i, sizeX));
    {name, map: emptyMap};
  };
};

module Tiles = {
  let groundTile = { tile: GROUND, state: Empty, tileEffect: NoEff, visible: false };
  let wallTile = { tile: WALL, state: Empty, tileEffect: NoEff, visible: false };
  let waterTile = { tile: WATER, state: Empty, tileEffect: NoEff, visible: false };

  let isGround = t => switch t {
    | GROUND => true
    | _ => false
    };

  let cantSeeThrough = t => switch t.tile {
    | WALL => true
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

  let hasEffect = t => switch t.tileEffect {
    | NoEff => false
    | _ => true
    };

  let isSnare = t => switch t.tileEffect {
    | Snare(_) => true
    | _ => false
    };

  let isGold = t => switch t.tileEffect {
    | Gold(_) => true
    | _ => false
    };

  let isHeal = t => switch t.tileEffect {
    | Heal(_) => true
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
    | ROUGH => 1.1
    | _ => 1.
    };

  let statePenalty = t => switch t {
    | Enemy(_) => 3.
    | _ => 0.
    };

  let objPenalty = (incTraps, t) => switch t {
    | Trap(_) => if (incTraps) 3. else 0.
    | Snare(_) => if (incTraps) 3. else 0.
    | _ => 0.
    };

  let placePenalty = (~incTraps=false, t)  => tilePenalty(t.tile) +. statePenalty(t.state) +. objPenalty(incTraps, t.tileEffect);

  let placePenaltyNoEnemy = t => tilePenalty(t.tile);

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
            | ROUGH => if (isEmpty(l) || overwrite) success(l) else error(ImpossibleMove)
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

    let updatePlayer = (player: player, tile) =>
      switch tile.tileEffect {
      | Trap(dmg) => 
        {... player, 
          stats: { ... player.stats, 
            health: player.stats.health - dmg,
            position: player.stats.position -. (Tiles.placePenalty(tile) *. cost)}};    
      | Heal(amt) => {
        {... player, 
          stats: { ... player.stats, 
            health: player.stats.health + amt,
            position: player.stats.position -. (Tiles.placePenalty(tile) *. cost)}};        
       }
      | Snare(turnCount) => {
        {... player, 
          stats: { ... player.stats, 
            position: player.stats.position -. (Tiles.placePenalty(tile) *. cost) -. turnCount }};        
       }
      | Gold(amt) => {
        {... player, 
          stats: { ... player.stats, 
            position: player.stats.position -. (Tiles.placePenalty(tile) *. cost)},
          gold: player.gold + amt
          };        
       }
      | NoEff => {... player, stats: { ... player.stats, position: player.stats.position -. (Tiles.placePenalty(tile) *. cost)}}
      };


    let nopFunc = (player: player, tile) => player;

    let update = (playerFunc, map) => {
      map |>
      List.mapi((xi: int, xs: list(place)) =>
        if (xi == y) {
            xs |> List.mapi((yi: int, place: place) =>
            if (yi == x) { 
              let np:player = playerFunc(place);
              { ...place, state: Player({ ...np, location: (x, y) }), tileEffect: NoEff }
            } else place);
        } else xs
      );
    };

    canMoveTo(x, y, area)
      |> Result.bind(_, _r =>
        findPlayer(area) 
        |> Option.fmap((p: player) => (updatePlayer(p)))
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
              { 
                let enemy = e(place);
                if (enemy.stats.health > 0) { ...place, state: Enemy(enemy), tileEffect: NoEff }
                else  { ...place, state: Empty, tileEffect: NoEff }
              }
            } else place);
        } else xs
      );
    };

    canMoveTo(x, y, area) 
      |> Result.fmap(_ => {
        let updatedEnemy = tile => switch(tile.tileEffect) {
        | Trap(dmg) => { ... enemy, 
            stats: { ... enemy.stats, 
              health: enemy.stats.health - dmg,
              position: (enemy.stats.position -. (cost *. Tiles.placePenaltyNoEnemy(tile)))}
          }
        | Snare(turnCount) => { ... enemy, 
          stats: { ... enemy.stats, 
            position: (enemy.stats.position -. (cost *. Tiles.placePenaltyNoEnemy(tile))) -. turnCount }
        }
        | _ => { ... enemy, 
            stats: { ... enemy.stats, position: (enemy.stats.position -. (cost *. Tiles.placePenaltyNoEnemy(tile)))}
          }
        }
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
            area 
              |> canMoveTo(~overwrite=false, nx, ny)
              >>= _ => 
              setPlayerLocation(nx, ny, cost, area)
                |> Result.fmap(removeOccupant(xl, yl)) 
                |> Result.bind(_, a => 
                  findPlayer(a) 
                    |> Option.fmap(p => { player: p, area: a})
                    |> Result.ofOption(InvalidState)
                  )
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
  
  let removeOccupant = (x, y, level) => { ...level, map: Area.removeOccupant(x, y, level.map) };
};
