open Types;
open Rationale;

module Enemies = {
  let randId = () => Js.Math.random() |> string_of_float;

  let makeZombie = () => { 
    id: randId(), name: "Zombie", 
    stats: { health: 6, speed: 0.8, position: 0., damage: 1 }, 
    ai: { moveRange: 4, terrainCost: false }};
  let makeEnemy = () => { 
    id: randId(), name: "Enemy", 
    stats: { health: 3, speed: 1., position: 0., damage: 2 },
    ai: { moveRange: 6, terrainCost: true }};
  let makeMinotaur = () => {
    id: randId(), name: "Minotaur", 
    stats: { health: 10, speed: 1., position: 0., damage: 3 },
    ai: { moveRange: 8, terrainCost: true }};

  let addEnemy = (str, place) => {
    switch str {
    | "Z" => { ... place, state: Enemy(makeZombie()) }
    | "X" => { ... place, state: Enemy(makeEnemy()) }
    | "M" => { ... place, state: Enemy(makeMinotaur()) }
    | _ => place
    };
  };
}

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
    /* let range = enemyInfo.enemy.ai.moveRange; */
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
        |> Option.fmap((player:player) => { ...player, stats: {...player.stats, health: player.stats.health - enemyInfo.enemy.stats.damage } } )
        >>= (player => Places.setPlayerAt(x, y, player, 0., area) 
          |> Option.ofResult 
          |> Option.fmap(area => (area, player)));
      } else {
      None
    };
  };

  let chase = (area, enemyInfo) => {
    let range = enemyInfo.enemy.ai.moveRange;
    let visiblePlaces = findTargets(~range=enemyInfo.enemy.ai.moveRange, enemyInfo);
    let playerLocations = attackablePlaces(visiblePlaces, area);

    if (List.length(playerLocations) == 0) (0, 0)
    else {
      let loc = List.hd(playerLocations);

      if(Pathing.Navigation.canNavigateTo(~limit=range, area, enemyInfo.location, loc))
        Pathing.Navigation.suggestMove(~limit=range, area, enemyInfo.location, loc)
      else (0, 0)
    }
  };

  let takeTurn = (activeEnemy, level, game) => {
    let canSee = canAttack(~range=activeEnemy.enemy.ai.moveRange);

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