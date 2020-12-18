open Types;
module Option = Belt_Option;

module Enemies = {
  let randId = () => Js.Math.random() |> Js.Float.toString;

  let makeZombie = () => {
    id: randId(),
    name: "Zombie",
    stats: {
      health: 5,
      speed: 0.7,
      position: 0.,
      damage: 1,
    },
    ai: {
      moveRange: 5,
      terrainCost: false,
      mustSee: true,
      memory: None,
      attackRange: 1,
      flying: false,
      swim: false,
      seedark: false,
      small: false,
    },
  };
  let makeEnemy = () => {
    id: randId(),
    name: "Enemy",
    stats: {
      health: 3,
      speed: 1.,
      position: 0.,
      damage: 2,
    },
    ai: {
      moveRange: 6,
      terrainCost: true,
      mustSee: true,
      memory: None,
      attackRange: 1,
      flying: false,
      swim: false,
      seedark: false,
      small: false,
    },
  };
  let makeMinotaur = () => {
    id: randId(),
    name: "Minotaur",
    stats: {
      health: 9,
      speed: 1.,
      position: 0.,
      damage: 3,
    },
    ai: {
      moveRange: 8,
      terrainCost: true,
      mustSee: false,
      memory: None,
      attackRange: 1,
      flying: false,
      swim: false,
      seedark: false,
      small: false,
    },
  };

  let addEnemy = (str, place) => {
    switch (str) {
    | "Z" => {...place, state: Enemy(makeZombie())}
    | "X" => {...place, state: Enemy(makeEnemy())}
    | "M" => {...place, state: Enemy(makeMinotaur())}
    | _ => place
    };
  };
};

module CreateEnemyLoop =
       (Pos: Types.Positions, Places: Types.Places, World: World) => {
  let findActiveEnemies = area =>
    area
    |> List.mapi((yi: int, xs: list(place)) =>
         xs
         |> List.mapi((xi: int, place: place) =>
              switch (place.state) {
              | Enemy(e) =>
                if (Pos.isActive(e.stats)) {
                  [{enemy: e, location: (xi, yi)}];
                } else {
                  [];
                }
              | _ => []
              }
            )
         |> List.flatten
       )
    |> List.flatten;

  let setEnemy = (~cost=1., area, enemyInfo) => {
    let (x, y) = enemyInfo.location;
    Places.setEnemyAt(x, y, enemyInfo.enemy, cost, area)
    ->Belt_Result.mapWithDefault(None, x => Some(x));
  };

  let updateEnemy = (area, enemyInfo, newLocation) => {
    let (x, y) = newLocation;
    let (ox, oy) = enemyInfo.location;

    let placeIsEmpty =
      Places.getPlace(x, y, area)
      ->Option.map(p =>
          switch (p.state) {
          | Empty => true
          | _ => false
          }
        )
      ->Option.getWithDefault(false);

    let moveIsPossible =
      Places.getPlace(x, y, area)
      ->Option.map(p =>
          switch (p.tile) {
          | WALL => false
          | _ => true
          }
        )
      ->Option.getWithDefault(false);

    if (placeIsEmpty && moveIsPossible) {
      Places.setEnemyAt(x, y, enemyInfo.enemy, 1., area)
      ->Belt_Result.mapWithDefault(None, x => Some(x))
      ->Option.map(Places.removeOccupant(ox, oy));
    } else {
      Places.setEnemyAt(ox, oy, enemyInfo.enemy, 1., area)
      ->Belt_Result.mapWithDefault(None, x => Some(x));
    };
  };

  let attackablePlaces = (targets, area) =>
    targets
    |> List.filter(pos => {
         let (x, y) = pos;
         Places.getPlace(x, y, area)
         ->Option.map(isPlayer)
         ->Option.getWithDefault(false);
       });

  let findTargets = (~range=1, enemyInfo) => {
    let (x, y) = enemyInfo.location;
    /* let range = enemyInfo.enemy.ai.moveRange; */
    let minX = x - range;
    let maxX = x + range;

    let append = (a, xs) => List.append(xs, [a]);
    let rangeF = {
      let rec loop = (inc, s, e, acc) =>
        switch (List.rev(acc)) {
        | [] => loop(inc, s, e, [s])
        | [a, ..._] =>
          inc(a) > e ? acc : loop(inc, s, e, append(inc(a), acc))
        };
      (inc, s, e) => loop(inc, s, e, []);
    };
    let rangeInt = step => rangeF(x => x + step);

    let incRange = rangeInt(1);

    let targetX = incRange(minX, maxX) |> List.filter(x => x >= 0);
    let targetY = incRange(y - range, y + range) |> List.filter(y => y >= 0);
    targetY
    |> List.map(y => targetX |> List.map(x => (x, y)))
    |> List.flatten
    |> List.filter(xy => xy != (x, y));
  };

  let canAttack = (~range=1, area, enemyInfo) => {
    let targets = findTargets(~range, enemyInfo);
    let attackable = attackablePlaces(targets, area);
    if (List.length(attackable) >= 1) {
      true;
    } else {
      false;
    };
  };

  let attack = (enemyInfo, area) => {
    let targets =
      enemyInfo |> findTargets(~range=1) |> attackablePlaces(_, area);
    if (List.length(targets) >= 1) {
      let (x, y) = List.hd(targets);
      let place = Places.getPlace(x, y, area);

      place
      -> Option.flatMap (
        place =>
          switch (place.state) {
          | Player(p) => Some(p)
          | _ => None
          }
      )
      -> Option.map((player: player) =>
           {
             ...player,
             stats: {
               ...player.stats,
               health: player.stats.health - enemyInfo.enemy.stats.damage,
             },
           }
         )
      -> Option.flatMap (
        player =>
          Places.setPlayerAt(x, y, player, 0., area)
          -> Belt_Result.mapWithDefault(None, x => Some(x))
          -> Option.map(area => (area, player))
      );
    } else {
      None;
    };
  };

  let chase = (area, enemyInfo) => {
    let enemyAi = enemyInfo.enemy.ai;
    let range = enemyAi.moveRange;
    let visiblePlaces = findTargets(~range, enemyInfo);
    let playerLocations = attackablePlaces(visiblePlaces, area);

    if (List.length(playerLocations) == 0) {
      ((0, 0), None);
    } else {
      let loc = List.hd(playerLocations);

      if (Pathing.Navigation.canNavigateTo(
            ~limit=range,
            area,
            enemyInfo.location,
            loc,
          )) {
        let move: (int, int) =
          Pathing.Navigation.suggestMove(
            ~limit=range,
            ~incTerrain=enemyAi.terrainCost,
            area,
            enemyInfo.location,
            loc,
          );
        (move, Some(loc));
      } else {
        ((0, 0), None);
      };
    };
  };

  let toLastSeen = (area, (mx, my), enemyInfo) => {
    let enemyAi = enemyInfo.enemy.ai;
    let range = enemyAi.moveRange;
    if (Pathing.Navigation.canNavigateTo(
          ~limit=range,
          area,
          enemyInfo.location,
          (mx, my),
        )) {
      Pathing.Navigation.suggestMove(
        ~limit=range,
        ~incTerrain=enemyAi.terrainCost,
        area,
        enemyInfo.location,
        (mx, my),
      );
    } else {
      (0, 0);
    };
  };

  let takeTurn = (activeEnemy, level, game) => {
    let canSee = ei => {
      let range = ei.enemy.ai.moveRange;
      if (ei.enemy.ai.mustSee) {
        Pathing.VisionUtil.canSee(
          ~limit=range,
          level.map,
          ei.location,
          game.player.location,
        );
      } else {
        canAttack(~range, level.map, ei);
      };
    };

    if (canAttack(
          ~range=activeEnemy.enemy.ai.attackRange,
          level.map,
          activeEnemy,
        )) {
      setEnemy(~cost=1., level.map, activeEnemy)
      -> Option.flatMap(_, map => attack(activeEnemy, map))
      -> Option.map(r => {
           let (area, player) = r;

           {...level, map: area}
           |> (
             lvl =>
               World.updateLevel(lvl, game.world)
               |> (w => {...game, world: w, player})
           );
         });
    } else if (canSee(activeEnemy)) {
      let ((dx, dy), mem) = chase(level.map, activeEnemy);
      let (ox, oy) = activeEnemy.location;

      let ne = {
        ...activeEnemy,
        enemy: {
          ...activeEnemy.enemy,
          ai: {
            ...activeEnemy.enemy.ai,
            memory: mem,
          },
        },
      };

      updateEnemy(level.map, ne, (ox + dx, oy + dy))
      -> Option.map(map => {...level, map})
      -> Option.map(l => World.updateLevel(l, game.world))
      -> Option.map(w => {...game, world: w});
    } else if (activeEnemy.enemy.ai.memory != None) {
      Js.Console.log("Using memory");
      let (mx, my) = activeEnemy.enemy.ai.memory -> Option.getWithDefault((0, 0));

      let (dx, dy) = toLastSeen(level.map, (mx, my), activeEnemy);
      let (ox, oy) = activeEnemy.location;
      let ne = {
        ...activeEnemy,
        enemy: {
          ...activeEnemy.enemy,
          ai: {
            ...activeEnemy.enemy.ai,
            memory: None,
          },
        },
      };
      updateEnemy(level.map, ne, (ox + dx, oy + dy))
      -> Option.map(map => {...level, map})
      -> Option.map(l => World.updateLevel(l, game.world))
      -> Option.map(w => {...game, world: w});
    } else {
      /* Wait / sleep */
      setEnemy(level.map, activeEnemy)
      -> Option.map(map => {...level, map})
      -> Option.map(l => World.updateLevel(l, game.world))
      -> Option.map(w => {...game, world: w});
    };
  };
};
