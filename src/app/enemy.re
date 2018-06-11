open Types;
open Rationale;

module CreateEnemyLoop = (Pos: Types.Positions, Places: Types.Places, World: World) => {

  let findActiveEnemies = area => 
    area |> List.mapi((yi: int, xs: list(place)) => 
      xs |> List.mapi((xi: int, place: place) => switch place.state {
      | Enemy(e) => if(Pos.isActive(e.stats)){[ { enemy: e, location: (xi, yi) }]} else {[]}
      | _ => []
      } ) |> List.flatten
    ) |> List.flatten;

  let setEnemy = (area, enemyInfo) => {
    let (x, y) = enemyInfo.location;
    Places.setEnemyAt(x, y, enemyInfo.enemy, 1., area) |> Rationale.Option.ofResult
  };

  let updateEnemy = (area, enemyInfo, newLocation) => {
    let (x, y) = newLocation;
    let (ox, oy) = enemyInfo.location;

    let placeIsEmpty = Places.getPlace(x, y, area) 
      |> Option.fmap(p => switch p.state {
        | Empty => true
        | _ => false })
      |> Option.default(false);

    let moveIsPossible = Places.getPlace(x, y, area) 
    |> Option.fmap(p => switch p.tile {
      | WALL => false
      | _ => true })
    |> Option.default(false);

    if (placeIsEmpty && moveIsPossible) {
      Places.setEnemyAt(x, y, enemyInfo.enemy, 1., area)
        |> Rationale.Option.ofResult
        |> Option.fmap(Places.removeOccupant(ox, oy))
    } else {
      Places.setEnemyAt(ox, oy, enemyInfo.enemy, 1., area) |> Rationale.Option.ofResult
    }
  };

  let attackablePlaces = (targets, area) => 
    targets 
      |> List.filter(pos => {
        let (x, y) = pos;
        Places.getPlace(x, y, area)
          |> Option.fmap(p => isPlayer(p))
          |> Option.default(false)
      }
    );

  let findTargets = (~range=1, enemyInfo) => {
    let (x, y) = enemyInfo.location;
    
    let minX = x - range;
    let maxX = x + range;
    let incRange = Rationale.RList.rangeInt(1);

    let targetX = incRange(minX, maxX) |> List.filter(x => x >= 0);
    let targetY = incRange((y-range), (y+range)) |> List.filter(y => y >= 0);
    targetY |> List.map(y => (targetX |> List.map(x => (x, y)))) |> List.flatten |> List.filter(xy => xy != (x, y));
  };

  let canAttack = (~range=1, area, enemyInfo) => {
    let targets = findTargets(~range=range, enemyInfo);
    let attackable = attackablePlaces(targets, area);

    if (List.length(attackable) >= 1) {
      true
    } else {
      false
    };
  };

  let attack = (enemyInfo, area) => {
    open Rationale.Option;
    let targets = enemyInfo |> findTargets |> attackablePlaces(_, area);
    if (List.length(targets) >= 1) {
      let (x,y) = List.hd(targets);
      let place = Places.getPlace(x, y, area);

      place >>= (place =>
          switch place.state {
            | Player(p) => Some(p)
            | _ => None;
          })
        |> Option.fmap((player:player) => { ...player, stats: {...player.stats, health: player.stats.health - 1} } )
        >>= (player => Places.setPlayerAt(x, y, player, 1., area) 
          |> Option.ofResult 
          |> Option.fmap(area => (area, player)));
      } else {
      None
    };
  };

  let chase = (~range=4, area, enemyInfo) => {
    let visiblePlaces = findTargets(~range=range, enemyInfo);
    let playerLocations = attackablePlaces(visiblePlaces, area);

    if (List.length(playerLocations) == 0) (0, 0)
    else {
      let loc = List.hd(playerLocations);
      let (px, py) = loc;
      let (ex, ey) = enemyInfo.location;

      let dx = px - ex;
      let dy = py - ey;

      let x = if(dx > 0) 1 else { if(dx == 0) 0 else -1 };
      let y = if(dy > 0) 1 else { if(dy == 0) 0 else -1 };

      (x, y)
    }
  };

  let takeTurn = (activeEnemy, level, game) => {
    let canSee = canAttack(~range=4);

    if (canAttack(level.map, activeEnemy)) {
      setEnemy(level.map, activeEnemy) 
        |> Option.bind(_, map => attack(activeEnemy, map))
        |> Option.fmap(r => {
        let (area, player) = r;

        {...level, map: area}
          |> lvl => World.updateLevel(lvl, game.world)
          |> w => {...game, world: w, player: player}
      })
    } else if (canSee(level.map, activeEnemy)) {

      let (dx, dy) = chase(level.map, activeEnemy);
      let (ox, oy) = activeEnemy.location;
      
      updateEnemy(level.map, activeEnemy, (ox + dx, oy + dy))
      |> Option.fmap(map => {...level, map: map })
      |> Option.fmap(l => World.updateLevel(l, game.world))
      |> Option.fmap(w => {...game, world: w})
    } else {
      /* Wait / sleep */
      setEnemy(level.map, activeEnemy) 
      |> Option.fmap(map => {...level, map: map })
      |> Option.fmap(l => World.updateLevel(l, game.world))
      |> Option.fmap(w => {...game, world: w})
    }
  };
};